/// create unique SSID using MAC address
pub fn from(mac:[u8;6]) -> heapless::String<32>
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
