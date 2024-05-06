#![no_std]
#![no_main]

// use panic_abort as _; // requires nightly
use panic_probe as _;
use defmt_rtt as _;

use defmt::*;
use heapless::Vec;
use static_cell::StaticCell;

use embassy_executor::Spawner;
use embassy_net_driver::Driver;
use embassy_net::Stack;
use embassy_net::udp::{PacketMetadata, UdpSocket};

use embassy_rp::bind_interrupts;
use embassy_rp::gpio::{Level, Output};
use embassy_rp::peripherals::{DMA_CH0, PIN_23, PIN_25, PIO0};
use embassy_rp::pio::{InterruptHandler, Pio};

bind_interrupts!(struct Irqs {
    PIO0_IRQ_0 => InterruptHandler<PIO0>;
});

#[embassy_executor::task]
async fn wifi_task(runner: cyw43::Runner<'static, Output<'static, PIN_23>, cyw43_pio::PioSpi<'static, PIN_25, PIO0, 0, DMA_CH0>>) -> !
{
    runner.run().await
}

#[embassy_executor::task]
async fn net_task(stack: &'static Stack<cyw43::NetDriver<'static>>) -> !
{
    stack.run().await
}

// captive portal DNS
#[embassy_executor::task]
async fn dns_task(stack: &'static Stack<cyw43::NetDriver<'static>>) -> !
{
    const MAX_PACKET_SIZE:usize = 1300;
    let mut rx_meta = [PacketMetadata::EMPTY; 16];
    let mut rx_buf = [0; MAX_PACKET_SIZE];
    let mut tx_meta = [PacketMetadata::EMPTY; 16];
    let mut tx_buf = [0; MAX_PACKET_SIZE];
    let mut captive_dns = UdpSocket::new(stack, &mut rx_meta, &mut rx_buf, &mut tx_meta, &mut tx_buf);
    captive_dns.bind(53).unwrap();

    loop {
        let mut buf = [0; MAX_PACKET_SIZE];
        let (n, ep) = captive_dns.recv_from(&mut buf).await.unwrap();
        let mut reply = [0; MAX_PACKET_SIZE];
        let len = sneakernet::dns_reply(&buf[..n], &mut reply);
        captive_dns.send_to(&reply[..len], ep).await.unwrap();
    }
}



#[embassy_executor::main]
async fn main(spawner: Spawner) {
    info!("SneakerNet starting....");

    // initialize RP2040 hardware
    let p = embassy_rp::init(Default::default());

    // initialize RP2040 WiFi hardware
    let pwr = Output::new(p.PIN_23, Level::Low);
    let cs = Output::new(p.PIN_25, Level::High);
    let mut pio = Pio::new(p.PIO0, Irqs);
    let spi = cyw43_pio::PioSpi::new(&mut pio.common, pio.sm0, pio.irq0, cs, p.PIN_24, p.PIN_29, p.DMA_CH0);
    static STATE: StaticCell<cyw43::State> = StaticCell::new();
    let state = STATE.init(cyw43::State::new());
    let fw = include_bytes!("../cyw43-firmware/43439A0.bin");
    let clm = include_bytes!("../cyw43-firmware/43439A0_clm.bin");
    let (net_device, mut control, runner) = cyw43::new(state, pwr, spi, fw).await;
    // FIXME unwrap!(spawner.spawn(wifi_task(runner)));
    spawner.spawn(wifi_task(runner)).unwrap();
    control.init(clm).await;
    control.set_power_management(cyw43::PowerManagementMode::PowerSave).await;

    // memo the hardware address to create the ssid
    let hw_addr= &net_device.hardware_address();

    // Init network stack
    let static_config_v4 = embassy_net::StaticConfigV4 {
       address: sneakernet::IP_ADDRESS,
       dns_servers: Vec::new(),
       gateway: None
    };
    let config = embassy_net::Config::ipv4_static(static_config_v4);
    static STACK: StaticCell<embassy_net::Stack<cyw43::NetDriver<'static>>> = StaticCell::new();
    static RESOURCES: StaticCell<embassy_net::StackResources<2>> = StaticCell::new();
    let seed = 0x511E_A11E_19ab_cdef;
    let stack = &*STACK.init(embassy_net::Stack::new(
        net_device,
        config,
        RESOURCES.init(embassy_net::StackResources::<2>::new()),
        seed,
    ));
    // FIXME unwrap!(spawner.spawn(net_task(stack)));
    spawner.spawn(net_task(stack)).unwrap();

    // create the captive portal DNS
    spawner.spawn(dns_task(stack)).unwrap();

    // create an open WiFi access point
    if let embassy_net_driver::HardwareAddress::Ethernet(mac) = hw_addr
    {
        // TODO choose an optimal channel
        let channel = 9;
        control.start_ap_open(
            core::str::from_utf8(&sneakernet::essid_from_mac(&mac)).unwrap(),
            channel)
        .await;
    }
    else { defmt::panic!("is not an ethernet device") }

}