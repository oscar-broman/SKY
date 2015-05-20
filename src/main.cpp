//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2007 SA:MP Team
//
//----------------------------------------------------------

#include "main.h"

#include "Addresses.h"
#include "Hooks.h"
#include "RPCs.h"

#include <fstream>
#include "Utils.h"
#include "Scripting.h"
#include "Functions.h"

#include <sdk/plugin.h>

#include "subhook/subhook.h"

#ifdef LINUX
#include <cstring>
typedef unsigned char *PCHAR;
#endif

//----------------------------------------------------------

void **ppPluginData;
extern void *pAMXFunctions;

// Internal server pointers
CNetGame *pNetGame = NULL;
void *pConsole = NULL;
RakServer *pRakServer = NULL;

//----------------------------------------------------------
// The Support() function indicates what possibilities this
// plugin has. The SUPPORTS_VERSION flag is required to check
// for compatibility with the server.

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

//----------------------------------------------------------
// The Load() function gets passed on exported functions from
// the SA-MP Server, like the AMX Functions and logprintf().
// Should return true if loading the plugin has succeeded.
typedef void(*logprintf_t)(char* format, ...);
logprintf_t logprintf;

int serverVersion = SAMP_VERSION_UNKNOWN;

PLUGIN_EXPORT bool PLUGIN_CALL Load(void ** ppData)
{
	ppPluginData = ppData;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

#ifndef _WIN32
	LoadTickCount();
#endif

	// Check server version
	serverVersion = SAMP_VERSION_UNKNOWN;
	char szVersion[64];

	if(logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03Z) {
		serverVersion = SAMP_VERSION_03Z;
		strcpy(szVersion, "0.3z");
	} else if(logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR2_2) {
		serverVersion = SAMP_VERSION_03Z_R2_2;
		strcpy(szVersion, "0.3z R2-2");
	} else if(logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR3) {
		serverVersion = SAMP_VERSION_03Z_R3;
		strcpy(szVersion, "0.3z R3");
	} else if(logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR4) {
		serverVersion = SAMP_VERSION_03Z_R4;
		strcpy(szVersion, "0.3z R4");
	} else if(logprintf == (logprintf_t)CAddress::FUNC_Logprintf_037RC1) {
		serverVersion = SAMP_VERSION_037RC1;
		strcpy(szVersion, "0.3.7 RC1");
	} else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_037) {
		serverVersion = SAMP_VERSION_037;
		strcpy(szVersion, "0.3.7");
	}

	if (1) {
		if (serverVersion == SAMP_VERSION_UNKNOWN) {
			logprintf("Error: Unknown " OS_NAME " server version\n");
			return true;
		}
	} else {
		serverVersion = SAMP_VERSION_037;
		strcpy(szVersion, "version check skipped");
	}

	InitRPCs();
	CAddress::Initialize();
	CSAMPFunctions::Initialize();
	InstallPreHooks();

	return 1;
}

//----------------------------------------------------------
// The Unload() function is called when the server shuts down,
// meaning this plugin gets shut down with it.

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

//----------------------------------------------------------
// The AmxLoad() function gets called when a new gamemode or
// filterscript gets loaded with the server. In here we register
// the native functions we like to add to the scripts.
typedef std::map<std::string, ConsoleVariable_s*> StringConvarMap;

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX * amx)
{
	static bool bFirst = false;

	if(!bFirst) {
		bFirst = true;

		// Get pNetGame
		int (*pfn_GetNetGame)(void) = (int(*)(void))ppPluginData[PLUGIN_DATA_NETGAME];
		pNetGame = (CNetGame*)pfn_GetNetGame();

		// Get pConsole
		int (*pfn_GetConsole)(void) = (int(*)(void))ppPluginData[PLUGIN_DATA_CONSOLE];
		pConsole = (void*)pfn_GetConsole();

		// Get pRakServer
		int (*pfn_GetRakServer)(void) = (int(*)(void))ppPluginData[PLUGIN_DATA_RAKSERVER];
		pRakServer = (RakServer*)pfn_GetRakServer();
	}

	return InitScripting(amx);
}

//----------------------------------------------------------
// When a gamemode is over or a filterscript gets unloaded, this
// function gets called. No special actions needed in here.

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX * amx)
{
	return AMX_ERR_NONE;
}

