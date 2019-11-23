#pragma once

// Fixes an annoying warning when building with MXE/MinGW "plugincommon.h"
// will display a warning about "Not using a VC++ compiler" not being set,
// since MXE is not windows.
// (This fixes multiple warnings, but it will still show once)
#if defined _MXE
#define _MSC_VER
#include <plugincommon.h>
#undef _MSC_VER
#else
#include <plugincommon.h>
#endif

#include "Addresses.h"
#include <amx/amx.h>

class CNetGame;
class RakServer;

extern CNetGame *pNetGame;
extern void *pConsole;
extern RakServer *pRakServer;
extern void **ppPluginData;
extern int iVersion;

typedef void (*logprintf_t)(const char *szFormat, ...);
extern logprintf_t logprintf;

#ifdef _WIN32
#define OS_NAME "Windows"
#else
#define OS_NAME "Linux"
#endif

#define PROJECT_NAME "SKY"
#define PROJECT_VERSION "2.1.0"
