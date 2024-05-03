#![no_std]



pub static IP_ADDRESS:embassy_net::Ipv4Cidr =
    embassy_net::Ipv4Cidr::new(embassy_net::Ipv4Address::new(192, 168, 4, 1), 24);

pub static SSID_PREFIX:&str = "SneakerNet";
pub fn essid(_mac:embassy_net_driver::HardwareAddress) -> &'static str {
    // TODO return "SneakerNet 01020304050607" (where hex represents the hardware MAC)
    return SSID_PREFIX;
}