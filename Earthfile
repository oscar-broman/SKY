VERSION 0.8

# Base image with build dependencies
ubuntu-base:
    FROM ubuntu:18.04
    ARG DEBIAN_FRONTEND=noninteractive

    # Install build dependencies
    RUN apt-get update && apt-get install -y \
        build-essential \
        cmake \
        make \
        git \
        wget \
        curl \
        pkg-config \
        gcc-multilib \
        g++-multilib \
        libc6-dev-i386 \
        && rm -rf /var/lib/apt/lists/*

    # Create gmake symlink for CMake compatibility
    RUN ln -s /usr/bin/make /usr/bin/gmake

    WORKDIR /workspace

# deps target - prepare dependencies and source
deps:
    FROM +ubuntu-base
    COPY . .

# build-linux target - builds for Linux
build-linux:
    FROM +deps
    RUN mkdir -p build
    RUN cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
    RUN cd build && cmake --build . --config Release
    SAVE ARTIFACT build/SKY.so AS LOCAL build/SKY.so

# all target - builds Linux only
all:
    BUILD +build-linux

# Define the CI target for Linux builds
ci:
    BUILD +build-linux

# export-artifacts target - exports built files to host
export-artifacts:
    FROM +build-linux
    SAVE ARTIFACT ./build/SKY.so AS LOCAL ./artifacts/linux/SKY.so

# package target - creates distribution packages
package:
    FROM +build-linux
    RUN mkdir -p dist
    COPY build/SKY.so dist/
    COPY SKY.inc dist/
    
    # Create release packages
    RUN cd dist && tar -czf release-linux.tar.gz SKY.so SKY.inc

    SAVE ARTIFACT dist/release-linux.tar.gz AS LOCAL dist/release-linux.tar.gz

# check-glibc target - check glibc version
check-glibc:
    FROM +ubuntu-base
    RUN ldd --version
