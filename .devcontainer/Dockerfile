FROM ubuntu:22.04

# Install basic tools and hexdump
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

# Install ARM GCC toolchain (GCC 14.2.1)
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && tar -xf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && mv arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi /opt/gcc-arm-none-eabi \
    && rm arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz

# Set environment
ENV PATH="/opt/gcc-arm-none-eabi/bin:${PATH}"

# Create a user (so you don't run as root)
RUN useradd -m devuser && echo "devuser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Setup SSH folder (mounted later, but make sure permissions are ready)
RUN mkdir -p /home/devuser/.ssh && chmod 700 /home/devuser/.ssh && chown devuser:devuser /home/devuser/.ssh

# Setup udev rules placeholder (optional if you mount /dev and want cleaner USB access)
RUN mkdir -p /etc/udev/rules.d

# Switch to devuser
USER devuser
WORKDIR /workspace
