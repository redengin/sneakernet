#![no_std]

use embassy_net::Ipv4Address;
pub const IP_ADDRESS:Ipv4Address = Ipv4Address::new(192,168,4,1);

// When you are okay with using a nightly compiler it's better to use https://docs.rs/static_cell/2.1.0/static_cell/macro.make_static.html
macro_rules! mk_static {
    ($t:ty,$val:expr) => {{
        static STATIC_CELL: static_cell::StaticCell<$t> = static_cell::StaticCell::new();
        #[deny(unused_attributes)]
        let x = STATIC_CELL.uninit().write(($val));
        x
    }};
}

pub fn start(spawner:embassy_executor::Spawner, wifi_interface:embassy_net::driver)
{
    // start the network stack
    let net_config = embassy_net::Config::ipv4_static(embassy_net::StaticConfigV4 {
        address: embassy_net::Ipv4Cidr::new(IP_ADDRESS, 24),
        gateway: None,
        dns_servers: Default::default(),
    });
    let net_stack = &*mk_static!(
        embassy_net::Stack<WifiDevice<'_, WifiApDevice>>,
        embassy_net::Stack::new(
            wifi_interface,
            net_config,
            mk_static!(embassy_net::StackResources<SOCKETS>, embassy_net::StackResources::<SOCKETS>::new()),
            Default::default(), // use default seed
        )
    );
    spawner.spawn(net_task(&net_stack)).ok();
}

#[embassy_executor::task]
async fn net_task(stack: &'static embassy_net::Stack<WifiDevice<'static, WifiApDevice>>) {
    stack.run().await
}