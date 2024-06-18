FROM rust:latest
# FROM rustlang/rust:nightly-slim

# add OS packages
#===============================================================================
RUN apt-get update && apt-get install -y \
    librust-libudev-dev \
 && rm -rf /var/lib/apt/lists/*

# add the toolchains for sneakernet targets
#===============================================================================
# support for ESP32 
#-------------------------------------------------------------------------------
# RUN cargo install espup && espup install
RUN curl -L https://github.com/esp-rs/espup/releases/latest/download/espup-x86_64-unknown-linux-gnu -o espup \
 && chmod a+x espup \
 && ./espup install \
 && rm espup
# Source the following file in every terminal before building a project
#. $HOME/export-esp.sh
ENV LIBCLANG_PATH="/usr/local/rustup/toolchains/esp/xtensa-esp32-elf-clang/esp-16.0.4-20231113/esp-clang/lib"
ENV PATH="/usr/local/rustup/toolchains/esp/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin:${PATH}"
RUN cargo install espflash


# support for RP2040
#-------------------------------------------------------------------------------
# RUN rustup target add thumbv6m-none-eabi \
#  && cargo install elf2uf2-rs
