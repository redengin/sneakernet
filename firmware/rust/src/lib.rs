#![no_std]

pub extern crate embassy_time;
pub extern crate embassy_net;
use embassy_net::Ipv4Address;

pub const IP_ADDRESS:Ipv4Address = Ipv4Address::new(192,168,4,1);

pub fn ssid(mac:[u8;6]) -> heapless::String<32>
{
    let mut ssid:heapless::String<32> = heapless::String::try_from("SneakerNet ").unwrap();
    for byte in mac {
        ssid.push(to_hex_char(byte/16)).unwrap();
        ssid.push(to_hex_char(byte%16)).unwrap();
    }
    return ssid;
}

fn to_hex_char(val:u8) -> char
{
    if val < 10 { return (('0' as u8) + val) as char};
    return (('A' as u8) + val - 10) as char;
}
