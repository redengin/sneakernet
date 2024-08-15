#![no_std]
#![no_main]

use esp_backtrace as _;
use esp_hal::{
    clock::ClockControl,
    peripherals::Peripherals,
    system::SystemControl,
    timer::{timg::TimerGroup, ErasedTimer, OneShotTimer, PeriodicTimer},
    rng::Rng,
};
use esp_wifi::wifi::{
    WifiApDevice,
    WifiDevice,
};

use embassy_executor::Spawner;
use sneakernet::embassy_time::{Duration, Timer};
use sneakernet::embassy_net::{
    Config,
    StaticConfigV4,
    Ipv4Cidr,
    Stack,
    StackResources,
    // tcp::TcpSocket,
    // IpListenEndpoint,
};

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
async fn main(spawner: Spawner) {
    let peripherals = Peripherals::take();
    let system = SystemControl::new(peripherals.SYSTEM);
    let clocks = ClockControl::boot_defaults(system.clock_control).freeze();

    // initialize embassy scheduler
    #[cfg(feature = "esp32")]
    {
        let timg1 = TimerGroup::new(peripherals.TIMG1, &clocks);
        let timer0: ErasedTimer = timg1.timer0.into();
        let timers = [OneShotTimer::new(timer0)];
        let static_timers = mk_static!([OneShotTimer<ErasedTimer>; 1], timers);
        esp_hal_embassy::init(&clocks, static_timers);
    }
    #[cfg(not(feature = "esp32"))]
    {
        let systimer = esp_hal::timer::systimer::SystemTimer::new(peripherals.SYSTIMER);
        let alarm0: ErasedTimer = systimer.alarm0.into();
        let timers = [OneShotTimer::new(alarm0)];
        let static_timers = mk_static!([OneShotTimer<ErasedTimer>; 1], timers);
        esp_hal_embassy::init(&clocks, static_timers);
    }

    // initialize wifi
    let wifi = peripherals.WIFI;
    let timg0 = TimerGroup::new(peripherals.TIMG0, &clocks);
    let timer0: ErasedTimer = timg0.timer0.into();
    let wifi_timer = PeriodicTimer::new(timer0);
    let wifi_init = esp_wifi::initialize(
        esp_wifi::EspWifiInitFor::Wifi,
        wifi_timer,
        Rng::new(peripherals.RNG),
        peripherals.RADIO_CLK,
        &clocks,
    ).unwrap();
    let (wifi_interface, mut wifi_controller) =
        esp_wifi::wifi::new_with_mode(&wifi_init, wifi, WifiApDevice).unwrap();
    let wifi_mac = wifi_interface.mac_address();

    // start the network stack
    let net_config = Config::ipv4_static(StaticConfigV4 {
        address: Ipv4Cidr::new(sneakernet::IP_ADDRESS, 24),
        gateway: None,
        dns_servers: Default::default(),
    });
    let net_stack = &*mk_static!(
        Stack<WifiDevice<'_, WifiApDevice>>,
        Stack::new(
            wifi_interface,
            net_config,
            mk_static!(StackResources<3>, StackResources::<3>::new()),
            Default::default(), // use default seed
        )
    );
    spawner.spawn(net_task(&net_stack)).ok();

    // start the wifi access point
    let ap_config = esp_wifi::wifi::AccessPointConfiguration{
        // ssid: "SneakerNet".try_into().unwrap(),
        ssid: sneakernet::ssid(wifi_mac),
        ..Default::default()
    };
    let wifi_config = esp_wifi::wifi::Configuration::AccessPoint(ap_config);
    wifi_controller.set_configuration(&wifi_config).unwrap();
    wifi_controller.start().await.unwrap();


    loop {
        esp_println::println!("PONG!");
        Timer::after(Duration::from_millis(500)).await;
    }
}


#[embassy_executor::task]
async fn net_task(stack: &'static Stack<WifiDevice<'static, WifiApDevice>>) {
    stack.run().await
}
