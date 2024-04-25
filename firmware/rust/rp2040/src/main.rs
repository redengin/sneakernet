#![no_std]
#![no_main]

use panic_abort as _; // resets upon panic (requires nightly)
use embassy_executor::Spawner;

// RP2040
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#[embassy_executor::main]
async fn main(spawner: Spawner)
{
    let p = embassy_rp::init(Default::default());
}

