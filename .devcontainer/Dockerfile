FROM ubuntu:22.04

# Install basic tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    gdb-multiarch \
    libncurses5-dev \
    python3 \
    python3-pip \
    wget \
    unzip \
    sudo \
    bash-completion \
    && apt-get clean

# Install ARM GCC toolchain
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 \
    && tar -xf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 \
    && mv gcc-arm-none-eabi-10.3-2021.10 /opt/gcc-arm-none-eabi \
    && rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

# Set environment
ENV PATH="/opt/gcc-arm-none-eabi/bin:${PATH}"

# Create a user (so you don't run as root)
RUN useradd -m devuser && echo "devuser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
USER devuser
WORKDIR /workspace
