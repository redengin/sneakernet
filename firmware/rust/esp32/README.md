# Rust Setup for Espressif SoCs
https://docs.esp-rs.org/book/installation/index.html
* (all targets) https://docs.esp-rs.org/book/installation/riscv-and-xtensa.html

# Supported Espressif Hardware
* **esp32** - generic support for xtensa based hardware
    * **esp32s2**, **esp32s3** - specific xtensa based hardware
* **esp32c2**, **esp32c3**, **esp32c6**, **esp32h2** - riscv based hardware

## Building
Use the label from the supported hardware for the `espvariant`.
```sh
cargo build <espvariant>.build 
```
## Flashing/Running
Use the label from the supported hardware for the `espvariant`.
```sh
cargo build <espvariant>.run
```


