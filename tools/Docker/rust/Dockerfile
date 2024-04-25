# FROM rust:latest
FROM rustlang/rust:nightly-slim

# add OS packages
RUN apt-get update && apt-get install -y \
    curl \
    xz-utils \
 && rm -rf /var/lib/apt/lists/*


# add debug/flashing support
RUN curl --proto '=https' --tlsv1.2 -LsSf https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-installer.sh | sh


# add the toolchains for sneakernet targets
#-------------------------------------------------------------------------------
# support for RP2040....
RUN rustup target add thumbv6m-none-eabi


# add support for ESP32 https://github.com/esp-rs/rust-build?tab=readme-ov-file
# RUN curl -L https://github.com/esp-rs/espup/releases/latest/download/espup-x86_64-unknown-linux-gnu -o espup \
#  && chmod a+x espup \
#  && ./espup install
#  # Source the following file in every terminal before building a project
#  #. $HOME/export-esp.sh