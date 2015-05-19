# This file demonstrates how to compile the SKY project on Linux.
#
# To compile SKY do:
#
# make SKY
#

GPP = g++ -m32 -Ilib -fno-stack-protector
GCC = gcc -m32 
SKY_OUTFILE = "./SKY.so"

COMPILE_FLAGS = -g -O3 -c -fPIC -w -DLINUX

SKY = -D SKY $(COMPILE_FLAGS)

all: SKY
03z: GPP += -DSAMP03Z
03z: SKY

clean:
	-rm -f *~ *.o *.so

SKY: clean
	$(GPP) $(SKY) ./lib/sdk/*.cpp
	$(GPP) $(SKY) ./lib/raknet/*.cpp
	$(GCC) $(SKY) ./lib/subhook/subhook.c
	$(GPP) $(SKY) ./src/*.cpp
	$(GCC) -g -nodefaultlibs -fshort-wchar -shared -o $(SKY_OUTFILE) *.o
