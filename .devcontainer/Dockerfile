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
    openssh-client \
    udev \
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

# Setup SSH folder (mounted later, but make sure permissions are ready)
RUN mkdir -p /home/devuser/.ssh && chmod 700 /home/devuser/.ssh && chown devuser:devuser /home/devuser/.ssh

# Setup udev rules placeholder (optional if you mount /dev and want cleaner USB access)
RUN mkdir -p /etc/udev/rules.d

# Switch to devuser
USER devuser
WORKDIR /workspace
