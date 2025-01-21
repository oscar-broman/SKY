FROM qmcgaw/basedevcontainer:debian

RUN dpkg --add-architecture i386 && apt-get update -y && apt-get install -y gcc g++ make cmake gcc-multilib g++-multilib wget curl
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin && chmod +x /usr/local/bin/task
RUN wget https://github.com/earthly/earthly/releases/latest/download/earthly-linux-amd64 -O /usr/local/bin/earthly \
    && chmod +x /usr/local/bin/earthly \
    && /usr/local/bin/earthly bootstrap --with-autocomplete