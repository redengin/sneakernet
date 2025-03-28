


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
    loop {
        _ = edge_dhcp::io::server::run(
            &mut Server::<_, 64>::new_with_et(IP_ADDRESS),
            &ServerOptions::new(IP_ADDRESS, None),
            &mut bound_socket,
            &mut buf,
        )
        .await
        .inspect_err(|e| log::error!("DHCP server error: {e:?}"))
        .unwrap();
    }
}