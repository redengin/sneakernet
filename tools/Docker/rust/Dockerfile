FROM rust:latest
# FROM rustlang/rust:nightly-slim

# add OS packages
RUN apt-get update && apt-get install -y \
    curl \
    xz-utils \
    librust-libudev-dev \
 && rm -rf /var/lib/apt/lists/*


# add debug/flashing support
RUN cargo install probe-rs --features=cli


# add the toolchains for sneakernet targets
#-------------------------------------------------------------------------------
# support for RP2040....
RUN rustup target add thumbv6m-none-eabi \
 && cargo install elf2uf2-rs
