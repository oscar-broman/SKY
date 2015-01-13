# This file demonstrates how to compile the SKY project on Linux.
#
# To compile SKY do:
#
# make SKY
#

GPP = g++ -m32 -Ilib
GCC = gcc -m32
SKY_OUTFILE = "./SKY.so"

COMPILE_FLAGS = -g -c -fPIC -w -D LINUX

SKY = -D SKY $(COMPILE_FLAGS)

all: SKY

clean:
	-rm -f *~ *.o *.so

SKY: clean
	$(GCC) $(SKY) ./lib/sdk/*.cpp
	$(GPP) $(SKY) ./lib/raknet/*.cpp
	$(GCC) $(SKY) ./lib/subhook/subhook.c
	$(GPP) $(SKY) ./src/*.cpp
	$(GPP) -g -fshort-wchar -shared -o $(SKY_OUTFILE) *.o
