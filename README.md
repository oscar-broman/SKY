# SKY - SA:MP Plugin

[![Build](https://github.com/oscar-broman/SKY/actions/workflows/build.yml/badge.svg)](https://github.com/oscar-broman/SKY/actions/workflows/build.yml)

SKY is a high-performance SA:MP (San Andreas Multiplayer) plugin that provides enhanced networking capabilities, anti-abuse features, and improved synchronization for SA:MP servers. Originally created by Slice and evolved from the YSF (Y_Less's Server Functions) plugin, SKY offers advanced packet handling, rate limiting, and player synchronization controls.

## Features

- **Enhanced Packet Handling**: Advanced packet validation and processing
- **Rate Limiting**: Configurable packet rate limits to prevent spam attacks
- **Anti-Abuse**: Invalid packet detection and automatic timeout mechanisms
- **Sync Control**: Granular control over player synchronization packets
- **Performance Optimized**: Built with modern C++17 standards for optimal performance
- **Cross-Platform**: Supports both Linux and Windows SA:MP servers

## Requirements

- SA:MP Server 0.3.7 or later
- Compatible with 0.3DL branch
- Linux: GCC with 32-bit support, CMake 3.10+
- Windows: Visual Studio 2017+ or Visual Studio Build Tools, CMake 3.10+

## Build Instructions

SKY supports multiple build methods to accommodate different development environments and deployment scenarios.

### Method 1: CMake + Make

#### Linux

Install gcc and g++, make and cmake. On Ubuntu you would do that like so:

```bash
sudo apt-get install gcc g++ make cmake
```

If you're on a 64-bit system you'll need additional packages for compiling
for 32-bit:

```bash
sudo apt-get install gcc-multilib g++-multilib
```

For CentOS:

```bash
yum install gcc gcc-c++ cmake28 make
```

Build the plugin:

```bash
cd SKY
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make
```

#### Windows

You'll need to install CMake and Visual Studio, once you have that
then you can run the command below to build:

```bash
cd SKY
mkdir build && cd build
cmake ../ -A Win32
cmake --build . --config Release
```

or you can open the source in Visual Studio, where it will autodetect that it uses CMake, and will
build a cache ready to be built.

### Method 2: Docker

For containerized builds and to ensure consistent build environments:

#### Prerequisites

- Docker installed on your system
- Make sure Docker daemon is running

#### Building with Docker

1. **Build the Docker image:**

   ```bash
   docker build -t sky-builder .
   ```

2. **Run the container and build:**

   ```bash
   # Mount your source code and build
   docker run --rm -v $(pwd):/workspace sky-builder bash -c "mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make"
   ```

3. **Or run interactively:**
   ```bash
   docker run -it --rm -v $(pwd):/workspace sky-builder
   # Inside container:
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make
   ```

The built `SKY.so` file will be available in the `build/` directory on your host system.

### Method 3: Earthly (Recommended for CI/CD)

Earthly provides reproducible builds across different environments and is excellent for CI/CD pipelines.

#### Prerequisites

- [Install Earthly](https://earthly.dev/get-earthly)

#### Building with Earthly

1. **Build for Linux:**

   ```bash
   earthly +build-linux
   ```

2. **Build all targets:**

   ```bash
   earthly +all
   ```

3. **Export artifacts to host:**

   ```bash
   earthly +export-artifacts
   ```

   This will create the `artifacts/linux/SKY.so` file.

4. **Create distribution packages:**
   ```bash
   earthly +package
   ```
   This creates a `dist/release-linux.tar.gz` with the plugin and include file.

#### Earthly Targets Available:

- `+build-linux` - Build the Linux version of the plugin
- `+all` - Build all supported platforms (currently Linux only)
- `+export-artifacts` - Export built files to the `artifacts/` directory
- `+package` - Create distribution packages
- `+check-glibc` - Check the glibc version in the build environment

### Method 4: Task Runner

If you have [Task](https://taskfile.dev/) installed, you can use the predefined tasks:

```bash
# Build for Linux
task build:linux

# Export artifacts
task export

# Create packages
task package

# Clean build artifacts
task clean
```

## Installation

### Method 1: Using sampctl (Recommended)

If you're using [sampctl](https://github.com/Southclaws/sampctl) for package management:

1. Install the plugin directly:

   ```bash
   sampctl install oscar-broman/SKY
   ```

2. Or add it to your `pawn.json` dependencies:

   ```json
   {
     "dependencies": ["oscar-broman/SKY"]
   }
   ```

   Then run:

   ```bash
   sampctl ensure
   ```

3. Include the plugin in your gamemode/filterscript:
   ```pawn
   #include <SKY>
   ```

sampctl will automatically download the appropriate binary for your platform and manage the include files.

### Method 2: Manual Installation

1. Copy the built `SKY.so` (Linux) or `SKY.dll` (Windows) to your SA:MP server's `plugins/` directory
2. Copy `SKY.inc` to your SA:MP server's `pawno/include/` directory
3. Add `SKY` to your server.cfg plugins line:
   ```
   plugins SKY
   ```
4. Include the plugin in your gamemode/filterscript:
   ```pawn
   #include <SKY>
   ```

## Usage Examples

### Basic Anti-Abuse Configuration

```pawn
#include <a_samp>
#include <SKY>

public OnGameModeInit()
{
    // Set maximum invalid packets before timeout
    SetMaxInvalidPacketsThreshold(5);

    // Set timeout duration (in milliseconds)
    SetInvalidPacketTimeout(30000); // 30 seconds

    // Set packet rate limit (max 100 packets per 1000ms)
    SetPacketRateLimit(100, 1000);

    // Disable sync bugs and exploits
    SetDisableSyncBugs(true);

    // Disable stealth knife sync exploit
    SetKnifeSync(false);

    return 1;
}
```

### Player Synchronization Control

```pawn
public OnPlayerConnect(playerid)
{
    // Spawn player for all others but not for himself (useful for spectating)
    SpawnPlayerForWorld(playerid);

    // Block key synchronization (useful for preventing key spamming)
    SetKeySyncBlocked(playerid, false);

    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    // Force send death packet (useful for custom death handling)
    SendDeath(playerid);

    return 1;
}
```

### Advanced Sync Manipulation

```pawn
// Freeze a player's position sync (useful for admin commands)
public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/freeze", true))
    {
        if(IsPlayerAdmin(playerid))
        {
            new targetid = 0; // Example target

            // Freeze player sync packets
            FreezeSyncPacket(targetid, E_PLAYER_SYNC, true);

            SendClientMessage(playerid, -1, "Player frozen!");
            return 1;
        }
    }

    if(!strcmp(cmdtext, "/unfreeze", true))
    {
        if(IsPlayerAdmin(playerid))
        {
            new targetid = 0; // Example target

            // Unfreeze player sync packets
            FreezeSyncPacket(targetid, E_PLAYER_SYNC, false);

            SendClientMessage(playerid, -1, "Player unfrozen!");
            return 1;
        }
    }

    return 0;
}
```

### Fake Health/Armour Display

```pawn
// Set fake health/armour values (visual only)
public OnPlayerSpawn(playerid)
{
    // Make player appear to have full health to others
    SetFakeHealth(playerid, 100.0);

    // Make player appear to have full armour to others
    SetFakeArmour(playerid, 100.0);

    // Set a fake facing angle
    SetFakeFacingAngle(playerid, 90.0);

    return 1;
}
```

### Custom TextDraw Positioning

```pawn
new Text:gMyTextDraw;

public OnGameModeInit()
{
    // Create a textdraw
    gMyTextDraw = TextDrawCreate(320.0, 240.0, "Hello World!");

    // Move textdraw position (enhanced function)
    TextDrawSetPosition(gMyTextDraw, 400.0, 300.0);

    return 1;
}

// Per-player textdraw content
public OnPlayerConnect(playerid)
{
    new string[128];
    format(string, sizeof(string), "Welcome %s!", GetPlayerNameEx(playerid));

    // Set different text for each player
    TextDrawSetStrForPlayer(gMyTextDraw, playerid, string);
    TextDrawShowForPlayer(playerid, gMyTextDraw);

    return 1;
}
```

### Animation and Sync Control

```pawn
// Send custom sync packets
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(newkeys & KEY_FIRE)
    {
        // Send last sync packet to all players
        for(new i = 0; i < MAX_PLAYERS; i++)
        {
            if(IsPlayerConnected(i) && i != playerid)
            {
                SendLastSyncPacket(playerid, i, E_PLAYER_SYNC, 1);
            }
        }
    }

    return 1;
}

// Clear animations for specific players
stock ClearPlayerAnimationForAll(playerid)
{
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(IsPlayerConnected(i) && i != playerid)
        {
            ClearAnimationsForPlayer(playerid, i);
        }
    }
}
```

### Anti-Abuse Integration

```pawn
// Enhanced anti-abuse with SKY features
public OnGameModeInit()
{
    // Configure anti-abuse settings
    SetMaxInvalidPacketsThreshold(3);        // Stricter threshold
    SetInvalidPacketTimeout(60000);          // 1 minute timeout
    SetPacketRateLimit(50, 1000);           // Conservative rate limit
    SetDisableSyncBugs(true);               // Block known exploits

    return 1;
}
```

## Credits

- Slice (SKY Creator)
- Kurta999 (Previous YSF Maintainer)
- Y_Less (The original YSF Creator)
- Illidan (Current YSF Maintainer)
- Whitetigerswt (0.3DL Work and Patches)
- BrunoBM16 (Packet/Network Patches)
- Alanskz/J0sh (Packet/Network/Sync Patches)
- NickDodd25 (Code Cleanup)
- NexiusTailer (Patches/Knowledge)
- najuaircrack (Patches)
- JustMichael/ADRFranklin (Maintainer)
