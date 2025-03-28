#![no_std]
// export common cargo
pub use log;
pub use static_cell;
pub use embassy_time;
pub use embassy_net;

/// create a run-once body of code (second run will cause panic)
// When you are okay with using a nightly compiler it's better to use https://docs.rs/static_cell/2.1.0/static_cell/macro.make_static.html
#[macro_export]
macro_rules! make_static {
    ($t:ty,$val:expr) => {{
        static STATIC_CELL: static_cell::StaticCell<$t> = static_cell::StaticCell::new();
        #[deny(unused_attributes)]
        let x = STATIC_CELL.uninit().write(($val));
        x
    }};
}

/// used to create WiFi SSIDs
pub mod ssid;

/// IP Address for SneakerNet node
pub const IP_ADDRESS:embassy_net::Ipv4Address = embassy_net::Ipv4Address::new(192,168,4,1);

/// create the sneakernet services
pub fn start(spawner:embassy_executor::Spawner, net_stack:embassy_net::Stack<'static>)
{
    // start dhcp service
    spawner.spawn(dhcp_service(net_stack)).unwrap();
    log::info!("DHCP service started");
}

/// dhcp server
mod dhcp_server;
#[embassy_executor::task]
async fn dhcp_service(net_stack: embassy_net::Stack<'static>)
{
    dhcp_server::run(net_stack).await;
    log::error!("DHCP server died");
}