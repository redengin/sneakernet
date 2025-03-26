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


/// create an SSID
pub fn ssid(mac:[u8;6]) -> heapless::String<32>
{
    let mut ssid:heapless::String<32> = heapless::String::try_from("SneakerNet").unwrap();
    // add the MAC address
    ssid.push(' ').unwrap();
    for byte in mac {
        ssid.push(hex_char(byte/16)).unwrap();
        ssid.push(hex_char(byte%16)).unwrap();
    }
    return ssid;
}
fn hex_char(val:u8) -> char
{
    if val < 10 { return (('0' as u8) + val) as char};
    return (('A' as u8) + val - 10) as char;
}


/// IP Address for SneakerNet node
pub const IP_ADDRESS:embassy_net::Ipv4Address = embassy_net::Ipv4Address::new(192,168,4,1);

pub fn start(spawner:embassy_executor::Spawner, net_stack:embassy_net::Stack<'static>)
{
    // start dhcp service
    spawner.spawn(dhcp_service(net_stack)).unwrap();
}

#[embassy_executor::task]
async fn dhcp_service(net_stack: embassy_net::Stack<'static>)
{
    log::debug!("DHCP service starting");
    use edge_nal_embassy::{UdpBuffers, Udp};
    let buffers = UdpBuffers::<3, 1024, 1024, 10>::new();
    let unbound_socket = Udp::new(net_stack, &buffers);

    use core::net::{SocketAddr, SocketAddrV4, Ipv4Addr};
    use edge_nal::UdpBind;
    let mut bound_socket = unbound_socket
        .bind(SocketAddr::V4(SocketAddrV4::new(
                Ipv4Addr::UNSPECIFIED,
                edge_dhcp::io::DEFAULT_SERVER_PORT,
        )))
        .await
        .unwrap();

    use edge_dhcp::server::{Server, ServerOptions};
    let mut buf = [0u8; 1500];
    log::debug!("DHCP service started");
    loop {
        log::debug!("waiting for dhcp request...");
        _ = edge_dhcp::io::server::run(
            &mut Server::<_, 64>::new_with_et(IP_ADDRESS),
            &ServerOptions::new(IP_ADDRESS, None),
            &mut bound_socket,
            &mut buf,
        )
        .await
        .inspect_err(|e| log::warn!("DHCP server error: {e:?}"));
        // Timer::after(Duration::from_millis(500)).await;
        log::debug!("dhcp request handled");
    }
}