#![no_std]



pub static address:embassy_net::Ipv4Cidr =
    embassy_net::Ipv4Cidr::new(embassy_net::Ipv4Address::new(192, 168, 4, 1), 24);

pub static ssid_prefix:&str = "SneakerNet";