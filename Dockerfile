FROM fedora:latest
USER root

RUN yum groupinstall "Development Tools" -y
RUN yum install libgcc.i686 glibc-devel.i686 gcc-c++ -y
RUN git clone -b 0.3DL --recurse-submodules -j2 https://github.com/oscar-broman/SKY.git

WORKDIR /SKY

COPY . /SKY

RUN make
RUN ld SKY.so && ldd SKY.so
