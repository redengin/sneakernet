#![no_std]
#![no_main]
// panic handler
use esp_backtrace as _;
use esp_alloc as _;

use sneakernet::{static_cell, make_static, embassy_net};
use sneakernet::embassy_net::StackResources;


#[esp_hal_embassy::main]
async fn main(spawner: embassy_executor::Spawner) {
    // initialize logger
    esp_println::logger::init_logger_from_env();

    // initialize the chip (max cpuclock required for wifi)
    let peripherals = esp_hal::init(
        esp_hal::Config::default().with_cpu_clock(esp_hal::clock::CpuClock::max())
    );

    // initialize the heap
    esp_alloc::heap_allocator!(72 * 1024);


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
    let mut rng = esp_hal::rng::Rng::new(peripherals.RNG);
    let wifi_init = &*make_static!(
        esp_wifi::EspWifiController<'static>,
        esp_wifi::init(timg0.timer0, rng.clone(), peripherals.RADIO_CLK).unwrap()
    );
    let (wifi_device, mut wifi_controller) =
        esp_wifi::wifi::new_with_mode(&wifi_init, peripherals.WIFI, esp_wifi::wifi::WifiApDevice).unwrap();

    // configure the AccessPoint
    let wifi_config = esp_wifi::wifi::Configuration::AccessPoint(
        esp_wifi::wifi::AccessPointConfiguration{
            ssid: sneakernet::ssid(wifi_device.mac_address()),
            ..Default::default()
        }
    );
    wifi_controller.set_configuration(&wifi_config).unwrap();

    // start the network stack
    let net_config = embassy_net::Config::ipv4_static(embassy_net::StaticConfigV4 {
        address: embassy_net::Ipv4Cidr::new(sneakernet::IP_ADDRESS, 24),
        gateway: None,
        dns_servers: Default::default(),
    });
    let seed = (rng.random() as u64) << 32 | rng.random() as u64;
    const SOCKETS_MAX:usize = 20; // TODO tune to optimize client support
    let (net_stack, runner) = embassy_net::new(
        wifi_device,
        net_config,
        make_static!(StackResources<{SOCKETS_MAX}>, StackResources::<{SOCKETS_MAX}>::new()),
        seed,
    );
    spawner.spawn(net_task(runner)).unwrap();

    // start sneakernet
    sneakernet::start(spawner, net_stack);

    // publish the wifi AP
    wifi_controller.start().unwrap();

    loop{};

}

#[embassy_executor::task]
async fn net_task(mut runner: embassy_net::Runner<'static, esp_wifi::wifi::WifiDevice<'static, esp_wifi::wifi::WifiApDevice>>) {
    runner.run().await
}