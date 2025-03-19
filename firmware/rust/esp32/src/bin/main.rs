#![no_std]
#![no_main]
// panic handler
use esp_backtrace as _;

use embassy_executor::Spawner;


// When you are okay with using a nightly compiler it's better to use https://docs.rs/static_cell/2.1.0/static_cell/macro.make_static.html
macro_rules! mk_static {
    ($t:ty,$val:expr) => {{
        static STATIC_CELL: static_cell::StaticCell<$t> = static_cell::StaticCell::new();
        #[deny(unused_attributes)]
        let x = STATIC_CELL.uninit().write(($val));
        x
    }};
}

#[esp_hal_embassy::main]
async fn main(_spawner: Spawner) {
    // initialize logger
    esp_println::logger::init_logger_from_env();

    // initialize the chip (max cpuclock required for wifi)
    let peripherals = esp_hal::init(
        esp_hal::Config::default().with_cpu_clock(esp_hal::clock::CpuClock::max())
    );

    // initialize embassy scheduler
#[cfg(feature = "esp32")] {
    let timg1 = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG1);
    esp_hal_embassy::init(timg1.timer0);
}
#[cfg(not(feature = "esp32"))] {
    use esp_hal::timer::systimer::SystemTimer;
    let systimer = SystemTimer::new(peripherals.SYSTIMER);
    esp_hal_embassy::init(systimer.alarm0);
}

    // initialize wifi hardware
    let timg0 = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG0);
    let rng = esp_hal::rng::Rng::new(peripherals.RNG);
    let wifi_init = &*mk_static!(
        esp_wifi::EspWifiController<'static>,
        esp_wifi::init(timg0.timer0, rng.clone(), peripherals.RADIO_CLK).unwrap()
    );
    let (wifi_interface, mut wifi_controller) =
        esp_wifi::wifi::new_with_mode(&wifi_init, peripherals.WIFI, esp_wifi::wifi::WifiApDevice).unwrap();




}