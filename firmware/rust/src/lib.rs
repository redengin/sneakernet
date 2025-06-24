#![no_std]
#![doc(html_favicon_url = "https://sneakernet.monster/favicon.ico")]
#![doc(issue_tracker_base_url = "https://github.com/redengin/sneakernet/issues/")]

// re-export shared cargo
pub use log;
pub use static_cell;
/// create a static allocation with runtime initialization
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

pub fn ssid_from_mac(mac: [u8; 6]) -> heapless::String<32> {
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


/// entrypoint for SneakerNet
/// called by hardware implementations, which pass their drivers in
///
/// panics upon failure
pub fn start() {}
