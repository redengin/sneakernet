#![no_std]
#![no_main]

use esp_backtrace as _;
use esp_hal::{
    peripherals::Peripherals,
    system::SystemControl,
    clock::ClockControl,
    timer::{timg::TimerGroup, ErasedTimer, OneShotTimer},
};

use embassy_executor::Spawner;
use embassy_time::{Duration, Timer};

// When you are okay with using a nightly compiler it's better to use https://docs.rs/static_cell/2.1.0/static_cell/macro.make_static.html
macro_rules! mk_static {
    ($t:ty,$val:expr) => {{
        static STATIC_CELL: static_cell::StaticCell<$t> = static_cell::StaticCell::new();
        #[deny(unused_attributes)]
        let x = STATIC_CELL.uninit().write(($val));
        x
    }};
}

#[embassy_executor::task]
async fn run() {
    loop {
        esp_println::println!("PING!");
        Timer::after(Duration::from_millis(500)).await;
    }
}

#[esp_hal_embassy::main]
async fn main(spawner: Spawner) {
    let peripherals = Peripherals::take();
    let system = SystemControl::new(peripherals.SYSTEM);
    let clocks = ClockControl::boot_defaults(system.clock_control).freeze();

    let timg0 = TimerGroup::new(peripherals.TIMG0, &clocks);
    let timer0: ErasedTimer = timg0.timer0.into();
    let timers = [OneShotTimer::new(timer0)];
    let timers = mk_static!([OneShotTimer<ErasedTimer>; 1], timers);
    esp_hal_embassy::init(&clocks, timers);

    spawner.spawn(run()).ok();

    loop {
        esp_println::println!("PONG!");
        Timer::after(Duration::from_millis(500)).await;
    }
}