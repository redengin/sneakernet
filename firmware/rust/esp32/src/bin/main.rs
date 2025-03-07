#![no_std]
#![no_main]
// panic handler
use esp_backtrace as _;

// use esp_hal::clock::CpuClock;
// use esp_hal::main;
// use esp_hal::time::{Duration, Instant};

// #[panic_handler]
// fn panic(_: &core::panic::PanicInfo) -> ! {
//     loop {}
// }

// #[main]
// fn main() -> ! {
//     // generator version: 0.3.1

//     let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
//     let _peripherals = esp_hal::init(config);

//     loop {
//         let delay_start = Instant::now();
//         while delay_start.elapsed() < Duration::from_millis(500) {}
//     }

//     // for inspiration have a look at the examples at https://github.com/esp-rs/esp-hal/tree/esp-hal-v1.0.0-beta.0/examples/src/bin
// }

// use embassy_executor::Spawner;
// use esp_hal::timer::timg::TimerGroup;
// use embassy_time::{Duration, Timer};

// #[embassy_executor::task]
// async fn run() {
//     loop {
//         esp_println::println!("Hello world from embassy using esp-hal-async!");
//         Timer::after(Duration::from_millis(1_000)).await;
//     }
// }

#[esp_hal_embassy::main]
async fn main(spawner: embassy_executor::Spawner) {
    esp_println::logger::init_logger_from_env();
    let peripherals = esp_hal::init(esp_hal::Config::default());

    esp_println::println!("Init!");

    // let timg0 = TimerGroup::new(peripherals.TIMG0);
    // esp_hal_embassy::init(timg0.timer0);

    // spawner.spawn(run()).ok();

    // loop {
    //     esp_println::println!("Bing!");
    //     Timer::after(Duration::from_millis(5_000)).await;
    // }
}