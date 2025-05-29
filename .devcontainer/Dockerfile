FROM ubuntu:22.04

# Install system dependencies
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
    openssh-client \
    udev \
    bsdmainutils \
    xz-utils \
    openocd \
    gcc-arm-none-eabi \
    usbutils \
    vim-common \
    stlink-tools \
    picocom \
    nano \
    && apt-get clean

# Install Python packages needed for imgtool
RUN pip3 install --upgrade pip && \
    pip3 install \
    "cryptography>=40.0.0" \
    intelhex \
    click \
    cbor2 \
    setuptools \
    pyyaml \
    pytest

# Install ARM GNU Toolchain (newer version)
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && tar -xf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && mv arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi /opt/gcc-arm-none-eabi \
    && rm arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz

# Set environment
ENV PATH="/opt/gcc-arm-none-eabi/bin:${PATH}"

# Create devuser and add to dialout and plugdev groups
RUN useradd -m -G dialout,plugdev devuser && echo "devuser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Prepare SSH folder (host will mount over it)
RUN mkdir -p /home/devuser/.ssh && chmod 700 /home/devuser/.ssh && chown devuser:devuser /home/devuser/.ssh

# Add udev rule for ST-Link (VID:PID 0483:374B)
RUN echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", GROUP="plugdev"' > /etc/udev/rules.d/49-stlinkv2.rules

# Set workspace
USER devuser
WORKDIR /workspace
