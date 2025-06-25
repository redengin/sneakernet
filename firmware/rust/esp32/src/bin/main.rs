#![no_std]
#![no_main]

/// implements panic
use esp_backtrace as _;
/// do a software reset upon panic
#[unsafe(no_mangle)]
pub extern "C" fn custom_halt() {
    esp_hal::reset::software_reset();
}
/// use the esp allocator
use esp_alloc as _;

use sneakernet::log;
/// provide the logging timestamp
#[no_mangle]
pub extern "Rust" fn _esp_println_timestamp() -> u64 {
    esp_hal::time::now().duration_since_epoch().to_millis()
}

use sneakernet::{make_static, static_cell};

#[esp_hal_embassy::main]
async fn main(spawner: embassy_executor::Spawner) {
    // initialize SoC (with max compute - aka cpu_clock frequency)
    let peripherals =
        esp_hal::init(esp_hal::Config::default().with_cpu_clock(esp_hal::clock::CpuClock::max()));

    // initialize logger
    #[cfg(debug_assertions)]
    esp_println::logger::init_logger(log::LevelFilter::Debug);
    #[cfg(not(debug_assertions))]
    esp_println::logger::init_logger(log::LevelFilter::Info);

    log::debug!("Initializing WiFi");
        esp_alloc::heap_allocator!(size: 72 * 1024);
    let timg0 = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG0);
    let mut rng = esp_hal::rng::Rng::new(peripherals.RNG);
    log::debug!("GOT HERE!");
    let wifi_controller = &*make_static!(
        esp_wifi::EspWifiController<'static>,
        esp_wifi::init(timg0.timer0, rng.clone(), peripherals.RADIO_CLK).unwrap()
    );
    let (wifi_ap, mut wifi_controller) = esp_wifi::wifi::new_with_mode(
        &wifi_controller,
        peripherals.WIFI,
        esp_wifi::wifi::WifiApDevice,
    )
    .unwrap();
    let wifi_max_channel: u8 = env!("WIFI_MAX_CHANNEL").parse().unwrap();
    let wifi_config =
        esp_wifi::wifi::Configuration::AccessPoint(esp_wifi::wifi::AccessPointConfiguration {
            ssid: sneakernet::ssid_from_mac(wifi_ap.mac_address()),
            channel: 1 + ((rng.random() as u8) % wifi_max_channel),
            secondary_channel: Some(1 + ((rng.random() as u8) % wifi_max_channel)),
            ..Default::default()
        });
    wifi_controller.set_configuration(&wifi_config).unwrap();
    wifi_controller.start().unwrap();

    // initialize embassy scheduler
    #[cfg(feature = "esp32")]
    {
        log::debug!("using TIMG1 for embassy scheduler");
        let timg1 = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG1);
        esp_hal_embassy::init(timg1.timer0);
    }
    #[cfg(not(feature = "esp32"))]
    {
        log::debug!("using SYSTIMER for embassy scheduler");
        use esp_hal::timer::systimer::SystemTimer;
        let systimer = SystemTimer::new(peripherals.SYSTIMER);
        esp_hal_embassy::init(systimer.alarm0);
    }

    // launch Sneakernet
    log::debug!("launching sneakernet");
    let seed = (rng.random() as u64) << 32 | rng.random() as u64;
    sneakernet::start(spawner, wifi_ap, seed);

    // FIXME
    // loop {}
}
