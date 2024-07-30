#![no_std]
#![no_main]
// handle exceptions and logging
use esp_backtrace as _;
use esp_println::println;
// ESP hardware
use esp_hal::{
    prelude::*,
    peripherals::Peripherals,
    system::SystemControl,
    clock::ClockControl,
    rng::Rng,
    timer::timg::TimerGroup,
};
use esp_wifi::{
    wifi::{
        WifiDevice,
        WifiApDevice,
        WifiController,
        WifiState,
        WifiEvent,
        Configuration,
        AccessPointConfiguration,
    },
    EspWifiInitFor,
};

use embassy_executor::Spawner;


// FIXME move embassy_net usage to sneakernet
use embassy_net::{
    StaticConfigV4,
    Stack,
    StackResources,
    Ipv4Cidr,
    // IpListenEndpoint,
    // tcp::TcpSocket,
};
use embassy_time::{Duration, Timer};

macro_rules! mk_static {
    ($t:ty,$val:expr) => {{
        static STATIC_CELL: static_cell::StaticCell<$t> = static_cell::StaticCell::new();
        #[deny(unused_attributes)]
        let x = STATIC_CELL.uninit().write(($val));
        x
    }};
}

#[main]
async fn main(spawner: Spawner) -> ! {
    let peripherals = Peripherals::take();
    let system = SystemControl::new(peripherals.SYSTEM);
    // freeze the clocks during initialization
    let clocks = ClockControl::max(system.clock_control).freeze();

    // initialize wifi
    #[cfg(target_arch = "xtensa")]
    let timer = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG1, &clocks, None).timer0;
    #[cfg(target_arch = "riscv32")]
    let timer = esp_hal::timer::systimer::SystemTimer::new(peripherals.SYSTIMER).alarm0;
    let wifi_init = esp_wifi::initialize(
        EspWifiInitFor::Wifi,
        timer,
        Rng::new(peripherals.RNG),
        peripherals.RADIO_CLK,
        &clocks,
    ).unwrap();
    let wifi = peripherals.WIFI;
    let (wifi_interface, controller) = esp_wifi::wifi::new_with_mode(&wifi_init, wifi, WifiApDevice).unwrap();

    // restart the clock
    let timer_group0 = TimerGroup::new_async(peripherals.TIMG0, &clocks);
    esp_hal_embassy::init(&clocks, timer_group0);

    // create the network stack
    let net_config = embassy_net::Config::ipv4_static(StaticConfigV4 {
        address: Ipv4Cidr::new(sneakernet::IP_ADDRESS, 24),
        gateway: None,
        dns_servers: Default::default(),
    });
    const MAX_SOCKETS:usize = 20;
    let stack = &*mk_static!(
        Stack<WifiDevice<'_, WifiApDevice>>,
        Stack::new(
            wifi_interface,
            net_config,
            mk_static!(StackResources<MAX_SOCKETS>, StackResources::<MAX_SOCKETS>::new()),
            Default::default(),
        )
    );

    // spawner.spawn(net_task(&stack)).ok();
    spawner.spawn(wifi_ap(controller)).ok();

    // await network stack initialization
    loop {
        if stack.is_link_up() {
            break;
        }
        Timer::after(Duration::from_millis(500)).await;
    }

    loop {}
}

// #[embassy_executor::task]
// async fn net_task(stack: &'static Stack<WifiDevice<'static, WifiApDevice>>) {
//     stack.run().await
// }

#[embassy_executor::task]
async fn wifi_ap(mut controller: WifiController<'static>) {
    println!("start connection task");
    println!("Device capabilities: {:?}", controller.get_capabilities());
    loop {
        match esp_wifi::wifi::get_wifi_state() {
            WifiState::ApStarted => {
                // wait until we're no longer connected
                controller.wait_for_event(WifiEvent::ApStop).await;
                Timer::after(Duration::from_millis(5000)).await
            }
            _ => {}
        }
        if !matches!(controller.is_started(), Ok(true)) {
            let client_config = Configuration::AccessPoint(AccessPointConfiguration {
                ssid: "esp-wifi".try_into().unwrap(),
                ..Default::default()
            });
            controller.set_configuration(&client_config).unwrap();
            println!("Starting wifi");
            controller.start().await.unwrap();
            println!("Wifi started!");
        }
    }
}

//     esp_println::logger::init_logger_from_env();

//     let peripherals = Peripherals::take();

//     let system = SystemControl::new(peripherals.SYSTEM);
//     let clocks = ClockControl::max(system.clock_control).freeze();

//     #[cfg(target_arch = "xtensa")]
//     let timer = esp_hal::timer::timg::TimerGroup::new(peripherals.TIMG1, &clocks, None).timer0;
//     #[cfg(target_arch = "riscv32")]
//     let timer = esp_hal::timer::systimer::SystemTimer::new(peripherals.SYSTIMER).alarm0;
//     let init = initialize(
//         EspWifiInitFor::Wifi,
//         timer,
//         Rng::new(peripherals.RNG),
//         peripherals.RADIO_CLK,
//         &clocks,
//     )
//     .unwrap();

//     let wifi = peripherals.WIFI;
//     let (wifi_interface, controller) =
//         esp_wifi::wifi::new_with_mode(&init, wifi, WifiApDevice).unwrap();

//     let timer_group0 = TimerGroup::new_async(peripherals.TIMG0, &clocks);
//     esp_hal_embassy::init(&clocks, timer_group0);

//     let config = Config::ipv4_static(StaticConfigV4 {
//         address: Ipv4Cidr::new(Ipv4Address::new(192, 168, 2, 1), 24),
//         gateway: Some(Ipv4Address::from_bytes(&[192, 168, 2, 1])),
//         dns_servers: Default::default(),
//     });

//     let seed = 1234; // very random, very secure seed

//     // Init network stack
//     let stack = &*mk_static!(
//         Stack<WifiDevice<'_, WifiApDevice>>,
//         Stack::new(
//             wifi_interface,
//             config,
//             mk_static!(StackResources<3>, StackResources::<3>::new()),
//             seed
//         )
//     );

//     spawner.spawn(connection(controller)).ok();
//     spawner.spawn(net_task(&stack)).ok();

//     let mut rx_buffer = [0; 1536];
//     let mut tx_buffer = [0; 1536];

//     loop {
//         if stack.is_link_up() {
//             break;
//         }
//         Timer::after(Duration::from_millis(500)).await;
//     }
//     println!("Connect to the AP `esp-wifi` and point your browser to http://192.168.2.1:8080/");
//     println!("Use a static IP in the range 192.168.2.2 .. 192.168.2.255, use gateway 192.168.2.1");

//     let mut socket = TcpSocket::new(&stack, &mut rx_buffer, &mut tx_buffer);
//     socket.set_timeout(Some(embassy_time::Duration::from_secs(10)));
//     loop {
//         println!("Wait for connection...");
//         let r = socket
//             .accept(IpListenEndpoint {
//                 addr: None,
//                 port: 8080,
//             })
//             .await;
//         println!("Connected...");

//         if let Err(e) = r {
//             println!("connect error: {:?}", e);
//             continue;
//         }

//         use embedded_io_async::Write;

//         let mut buffer = [0u8; 1024];
//         let mut pos = 0;
//         loop {
//             match socket.read(&mut buffer).await {
//                 Ok(0) => {
//                     println!("read EOF");
//                     break;
//                 }
//                 Ok(len) => {
//                     let to_print =
//                         unsafe { core::str::from_utf8_unchecked(&buffer[..(pos + len)]) };

//                     if to_print.contains("\r\n\r\n") {
//                         print!("{}", to_print);
//                         println!();
//                         break;
//                     }

//                     pos += len;
//                 }
//                 Err(e) => {
//                     println!("read error: {:?}", e);
//                     break;
//                 }
//             };
//         }

//         let r = socket
//             .write_all(
//                 b"HTTP/1.0 200 OK\r\n\r\n\
//             <html>\
//                 <body>\
//                     <h1>Hello Rust! Hello esp-wifi!</h1>\
//                 </body>\
//             </html>\r\n\
//             ",
//             )
//             .await;
//         if let Err(e) = r {
//             println!("write error: {:?}", e);
//         }

//         let r = socket.flush().await;
//         if let Err(e) = r {
//             println!("flush error: {:?}", e);
//         }
//         Timer::after(Duration::from_millis(1000)).await;

//         socket.close();
//         Timer::after(Duration::from_millis(1000)).await;

//         socket.abort();
//     }
// }

// #[embassy_executor::task]
// async fn connection(mut controller: WifiController<'static>) {
//     println!("start connection task");
//     println!("Device capabilities: {:?}", controller.get_capabilities());
//     loop {
//         match esp_wifi::wifi::get_wifi_state() {
//             WifiState::ApStarted => {
//                 // wait until we're no longer connected
//                 controller.wait_for_event(WifiEvent::ApStop).await;
//                 Timer::after(Duration::from_millis(5000)).await
//             }
//             _ => {}
//         }
//         if !matches!(controller.is_started(), Ok(true)) {
//             let client_config = Configuration::AccessPoint(AccessPointConfiguration {
//                 ssid: "esp-wifi".try_into().unwrap(),
//                 ..Default::default()
//             });
//             controller.set_configuration(&client_config).unwrap();
//             println!("Starting wifi");
//             controller.start().await.unwrap();
//             println!("Wifi started!");
//         }
//     }
// }

// #[embassy_executor::task]
// async fn net_task(stack: &'static Stack<WifiDevice<'static, WifiApDevice>>) {
//     stack.run().await
// }