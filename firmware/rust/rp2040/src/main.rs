#![no_std]
#![no_main]

// use panic_abort as _; // requires nightly
use panic_probe as _;
use defmt_rtt as _;

use heapless::Vec;

use cyw43_pio::PioSpi;
use defmt::*;
use embassy_executor::Spawner;
use embassy_rp::bind_interrupts;
use embassy_rp::gpio::{Level, Output};
use embassy_rp::peripherals::{DMA_CH0, PIN_23, PIN_25, PIO0};
use embassy_rp::pio::{InterruptHandler, Pio};
// use embassy_time::{Duration, Timer};
use static_cell::StaticCell;

bind_interrupts!(struct Irqs {
    PIO0_IRQ_0 => InterruptHandler<PIO0>;
});


#[embassy_executor::task]
async fn wifi_task(runner: cyw43::Runner<'static, Output<'static, PIN_23>, PioSpi<'static, PIN_25, PIO0, 0, DMA_CH0>>)
{
    runner.run().await
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
    (spawner.spawn(wifi_task(runner))).unwrap();
    control.init(clm).await;
    control.set_power_management(cyw43::PowerManagementMode::PowerSave).await;

    // Init network stack
    let static_config_v4 = embassy_net::StaticConfigV4 {
       address: sneakernet::IP_ADDRESS,
       dns_servers: Vec::new(),
       gateway: None
    };
    let config = embassy_net::Config::ipv4_static(static_config_v4);
    static STACK: StaticCell<embassy_net::Stack<cyw43::NetDriver<'static>>> = StaticCell::new();
    static RESOURCES: StaticCell<embassy_net::StackResources<2>> = StaticCell::new();
    let seed = 0x0123_4567_89ab_cdef; // TODO wtf is this for?
    let stack = &*STACK.init(embassy_net::Stack::new(
        net_device,
        config,
        RESOURCES.init(embassy_net::StackResources::<2>::new()),
        seed,
    ));
    // stack.run().await;

    // // create an open WiFi access point
    // // TODO choose an optimal channel
    // let channel = 9;
    // control.start_ap_open(
    //     core::str::from_utf8(&sneakernet::essid(net_device)).unwrap(),
    //     channel)
    //     .await;

}

    // // create an open WiFi access point
    // // TODO choose an optimal channel
    // let channel = 9;
    // control.start_ap_open(
    //     core::str::from_utf8(&sneakernet::essid(net_device)).unwrap(),
    //     channel)
    //     .await;


