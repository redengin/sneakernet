#![no_std]
#![no_main]
// #![allow(async_fn_in_trait)]

// debugging support
// use panic_abort as _;   // abort upon panic (TODO change for testing)
use panic_probe as _;   // log to probe
use defmt_rtt as _;     // use defmt for probe logging
// use core::str::from_utf8;
use defmt::*;

// Embassy support
use embassy_executor::Spawner;
use static_cell::StaticCell;
use embassy_net::{Config, Stack, StackResources};
// use embassy_net::tcp::TcpSocket;
// use embassy_time::{Duration, Timer};
// use embedded_io_async::Write;

// RP2040 support
use embassy_rp::bind_interrupts;
use embassy_rp::gpio::{Level, Output};
use embassy_rp::pio::{InterruptHandler, Pio};
use embassy_rp::peripherals::{DMA_CH0, PIO0};
use cyw43_pio::PioSpi;

// TODO move to sneakernet
use embassy_net::{Ipv4Address, Ipv4Cidr};
#[macro_use] extern crate fixedvec;

bind_interrupts!(struct Irqs {
    PIO0_IRQ_0 => InterruptHandler<PIO0>;
});


// #[embassy_executor::task]
// async fn net_task(stack: &'static Stack<cyw43::NetDriver<'static>>) -> ! {
//     stack.run().await
// }

#[embassy_executor::main]
async fn main(spawner: Spawner) {
    info!("SneakerNet starting....");

    // initialize RP2040 hardware
    let p = embassy_rp::init(Default::default());

    // initialize RP2040 WiFi
    let pwr = Output::new(p.PIN_23, Level::Low);
    let cs = Output::new(p.PIN_25, Level::High);
    let mut pio = Pio::new(p.PIO0, Irqs);
    let spi = PioSpi::new(&mut pio.common, pio.sm0, pio.irq0, cs, p.PIN_24, p.PIN_29, p.DMA_CH0);
    static STATE: StaticCell<cyw43::State> = StaticCell::new();
    let state = STATE.init(cyw43::State::new());
    let fw = include_bytes!("../cyw43-firmware/43439A0.bin");
    let clm = include_bytes!("../cyw43-firmware/43439A0_clm.bin");
    let (net_device, mut control, runner) = cyw43::new(state, pwr, spi, fw).await;
    runner.run().await;
    control.init(clm).await;
    control
        .set_power_management(cyw43::PowerManagementMode::PowerSave)
        .await;

    // Init network stack
    // TODO move to sneakernet
    let mut dns_servers_space = alloc_stack!([u8; 100 /* FIXME */]);
    let config = embassy_net::Config::ipv4_static(embassy_net::StaticConfigV4 {
       address: Ipv4Cidr::new(Ipv4Address::new(192, 168, 69, 2), 24),
       dns_servers: Vec::new(),
       gateway: Some(Ipv4Address::new(192, 168, 69, 1)),
    });
    static STACK: StaticCell<Stack<cyw43::NetDriver<'static>>> = StaticCell::new();
    static RESOURCES: StaticCell<StackResources<2>> = StaticCell::new();
    let seed = 0x0123_4567_89ab_cdef; // TODO wtf is this for?
    let stack = &*STACK.init(Stack::new(
        net_device,
        config,
        RESOURCES.init(StackResources::<2>::new()),
        seed,
    ));
    // unwrap!(spawner.spawn(net_task(stack)));
}