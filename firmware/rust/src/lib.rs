#![no_std]

// export common cargo
pub use static_cell;
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
    let mut ssid:heapless::String<32> = heapless::String::try_from("SneakerNet ").unwrap();
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

/// IP Address of SneakerNet node
pub const IP_ADDRESS:embassy_net::Ipv4Address = embassy_net::Ipv4Address::new(192,168,4,1);

// pub fn start(spawner:embassy_executor::Spawner, wifi_interface:embassy_net::driver)
pub fn start(_spawner:embassy_executor::Spawner)
{

}



// #[embassy_executor::task]
// async fn net_task(stack: &'static embassy_net::Stack<WifiDevice<'static, WifiApDevice>>) {
//     stack.run().await
// }