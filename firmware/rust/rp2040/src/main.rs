#![no_std]
#![no_main]
// #![allow(async_fn_in_trait)]

// use core::fmt::Debug;

use embassy_net::driver::Driver;
// debugging support
use panic_abort as _;   // abort upon panic (TODO change for testing)
use defmt_rtt as _;     // use defmt for probe logging
// use core::str::from_utf8;
use defmt::*;

// Embassy support
use embassy_executor::Spawner;
use static_cell::StaticCell;
use heapless::Vec;

// RP2040 support
use embassy_rp::bind_interrupts;
use embassy_rp::gpio::{Level, Output};
use embassy_rp::pio::{InterruptHandler, Pio};
use embassy_rp::peripherals::{DMA_CH0, PIO0};
use cyw43::{State};
use cyw43_pio::PioSpi;


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
    static STATE: StaticCell<State> = StaticCell::new();
    let state = STATE.init(State::new());
    let fw = include_bytes!("../cyw43-firmware/43439A0.bin");
    let clm = include_bytes!("../cyw43-firmware/43439A0_clm.bin");
    let (net_device, mut control, runner) = cyw43::new(state, pwr, spi, fw).await;
    runner.run().await;
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
    stack.run().await;


    // TODO choose an optimal channel
    let channel = 9;
    control.start_ap_open(
        sneakernet::essid(net_device.hardware_address()),
        channel)
        .await;

}