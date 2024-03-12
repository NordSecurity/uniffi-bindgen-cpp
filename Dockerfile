FROM gcc:10-bullseye

LABEL org.opencontainers.image.source=https://github.com/NordSecurity/uniffi-bindgen-cpp

RUN apt-get update && apt-get install -y --no-install-recommends cmake curl valgrind && apt-get clean

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain=1.72

RUN wget https://go.dev/dl/go1.22.1.linux-amd64.tar.gz \
    && tar -C /usr/local -xzf go1.22.1.linux-amd64.tar.gz \
    && echo 'export PATH=/usr/local/go/bin:$PATH' >> /root/.bashrc
