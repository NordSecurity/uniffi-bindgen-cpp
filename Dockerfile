FROM gcc:10-bullseye

LABEL org.opencontainers.image.source=https://github.com/NordSecurity/uniffi-bindgen-cpp

RUN apt-get update && apt-get install -y --no-install-recommends cmake curl valgrind && apt-get clean

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain=1.72
