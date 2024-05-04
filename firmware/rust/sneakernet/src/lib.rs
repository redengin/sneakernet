#![no_std]


pub static IP_ADDRESS:embassy_net::Ipv4Cidr =
    embassy_net::Ipv4Cidr::new(embassy_net::Ipv4Address::new(192, 168, 4, 1), 24);


const SSID_PREFIX:&[u8] = "SneakerNet".as_bytes();
pub fn essid<NetDevice:embassy_net_driver::Driver>(net_device:NetDevice) -> [u8; SSID_PREFIX.len() + 1 + (2*6)] {
    // FIXME return "SneakerNet 01020304050607" (where hex represents the hardware MAC)
    let mut essid = [0u8; SSID_PREFIX.len() + 1 + (2*6)];
    essid.copy_from_slice(SSID_PREFIX);
    essid[SSID_PREFIX.len()] = b' ';

    if let embassy_net_driver::HardwareAddress::Ethernet(mac) = net_device.hardware_address()
    {
        for i in 0..5
        {
            essid[SSID_PREFIX.len() + 1 + (2*i)] = hex(mac[i]/16);
            essid[SSID_PREFIX.len() + 1 + (2*i)+1] = hex(mac[i]%16);
        }
    }
    else { panic!("was not an ethernet device") };

    // let hw_addr = net_device.hardware_address();


    return essid;
}

fn hex(v:u8) -> u8
{
    if v < 10 { b'0' + v }
    else { b'A' + (v - 10) }
}
