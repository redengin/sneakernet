#![no_std]

use core::time::Duration;


pub static IP_ADDRESS:embassy_net::Ipv4Cidr =
    embassy_net::Ipv4Cidr::new(embassy_net::Ipv4Address::new(192, 168, 4, 1), 24);


const SSID_PREFIX:&[u8] = "SneakerNet".as_bytes();

pub fn essid_from_mac(mac:&[u8;6]) -> [u8; SSID_PREFIX.len() + 1 + (2*6)]
{
    let mut essid = [0u8; SSID_PREFIX.len() + 1 + (2*6)];
    // set the prefix
    essid.copy_from_slice(SSID_PREFIX);
    essid[SSID_PREFIX.len()] = b' ';

    // set the unique identifier
    for i in 0..5
    {
        essid[SSID_PREFIX.len() + 1 + (2*i)] = hex(mac[i]/16);
        essid[SSID_PREFIX.len() + 1 + (2*i)+1] = hex(mac[i]%16);
    }

    return essid;
}

fn hex(v:u8) -> u8
{
    if v < 10 { b'0' + v }
    else { b'A' + (v - 10) }
}

/// captive portal DNS response (always refer back to sneakernet::IP_ADDRESS)
pub fn dns_reply(
    request: &[u8],
    buf: &mut [u8],
) -> usize
{
    let octets:[u8; 4] = IP_ADDRESS.address().as_bytes().try_into().unwrap();
    return match edge_captive::reply(request, &octets, Duration::from_secs(60), buf)
    {
        Ok(len) => len,
        _ => 0
    }
}