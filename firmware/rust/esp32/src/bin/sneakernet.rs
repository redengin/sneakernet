#![no_std]
#![no_main]

use esp_backtrace as _;

use embassy_executor::Spawner;


#[esp_hal_embassy::main]
async fn main(spawner: Spawner) {
    loop {
        esp_println::println!("Bing!");
    }
}