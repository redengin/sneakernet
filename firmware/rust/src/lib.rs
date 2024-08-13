#![no_std]

pub extern crate embassy_time;
pub extern crate embassy_net;

use embassy_net::Ipv4Address;

pub const IP_ADDRESS:Ipv4Address = Ipv4Address::new(192,168,4,1);