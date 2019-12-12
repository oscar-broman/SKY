FROM dockcross/linux-x86:latest
COPY . /work
RUN chmod +x /work/build.sh
RUN cmake -DCMAKE_BUILD_TYPE=Release .
ENTRYPOINT ["/work/build.sh"]