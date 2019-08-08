#ifndef __YSF_MAINH
#define __YSF_MAINH

#include "Structs.h"
#include "Addresses.h"

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

#include <amx/amx.h>

class CNetGame;
class RakServer;

extern CNetGame *pNetGame;
extern void *pConsole;
extern RakServer *pRakServer;
extern void **ppPluginData;

typedef void(*logprintf_t)(const char * szFormat, ...);
extern logprintf_t logprintf;

// Defines
#define CON_VARFLAG_DEBUG		1
#define CON_VARFLAG_READONLY	2
#define CON_VARFLAG_RULE		4

#ifdef _WIN32
#define OS_NAME			"Windows"
#else
#define OS_NAME			"Linux"
#endif

#define PROJECT_NAME		"SKY"
#define PROJECT_VERSION		"0.2"

#define SAFE_DELETE(p)	{ if (p) { delete (p); (p) = NULL; } }

////
// Slight hack to load the correct structs on runtime
// and not have to have 2 plugin version
////
struct without_dl
{
#include "Structs.h"
};

struct with_dl
{
#define SAMP_03DL
#include "Structs.h"
};

#endif
