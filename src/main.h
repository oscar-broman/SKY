#pragma once

#include <plugincommon.h>
#include "Addresses.h"
#include <amx/amx.h>

class CNetGame;
class RakServer;

extern CNetGame *pNetGame;
extern void *pConsole;
extern RakServer *pRakServer;
extern void **ppPluginData;
extern eSAMPVersion iVersion;

typedef void (*logprintf_t)(const char *szFormat, ...);
extern logprintf_t logprintf;

#ifdef _WIN32
#define OS_NAME "Windows"
#else
#define OS_NAME "Linux"
#endif

#define PROJECT_NAME "SKY"
#define PROJECT_VERSION "2.3.0"
