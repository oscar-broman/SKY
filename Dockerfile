FROM ubuntu:18.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update package list and install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    pkg-config \
    gcc-multilib \
    g++-multilib \
    libc6-dev-i386 \
    && rm -rf /var/lib/apt/lists/*

# Install a more recent version of CMake if needed
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
    echo 'deb https://apt.kitware.com/ubuntu/ bionic main' > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Create a build user to avoid running as root
# Use configurable UID/GID to match host user
ARG USER_ID=1000
ARG GROUP_ID=1000

RUN groupadd -g $GROUP_ID builder && \
    useradd -l -u $USER_ID -g $GROUP_ID -m -s /bin/bash builder && \
    chown -R builder:builder /workspace

USER builder

CMD ["/bin/bash"]
