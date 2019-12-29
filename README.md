[![Build status](https://ci.appveyor.com/api/projects/status/2d3kodm5c9tfjw19/branch/0.3DL?svg=true)](https://ci.appveyor.com/project/Whitetigerswt/sky/branch/0.3DL)

## Build Instructions

### Linux

Install gcc and g++, make and cmake. On Ubuntu you would do that like so:

```
sudo apt-get install gcc g++ make cmake
```

If you're on a 64-bit system you'll need additional packages for compiling
for 32-bit:

```
sudo apt-get install gcc-multilib g++-multilib
```

For CentOS:

```
yum install gcc gcc-c++ cmake28 make
```

```
cd SKY
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make
```

### Windows

You'll need to install CMake and Visual Studio, once you have that
then you can run the command below to build

```
cd SKY
mkdir build && cd build
cmake ../ -A Win32
cmake --build . --config Release
```

or you can open the source in Visual Studio, where it will autodetect that it uses CMake, and will
build a cache ready to be built

## Credits

- Slice (SKY Creator)
- Kurta999 (Previous YSF Maintainer)
- Y_Less (The original YSF Creator)
- Illidan (Current YSF Maintainer)
- Whitetigerswt (0.3DL Work and Patches)
- BrunoBM16 (Packet/Network Patches)
- Alanskz/J0sh (Packet/Network/Sync Patches)
- NickDodd25 (Code Cleanup)
- JustMichael/ADRFranklin (Maintainer)
