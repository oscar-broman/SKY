FROM dockcross/linux-x86:latest
COPY . /work
ARG build=Debug

RUN chmod +x /work/build.sh
RUN cmake -DCMAKE_BUILD_TYPE=${build} .

ENTRYPOINT ["/work/build.sh"]