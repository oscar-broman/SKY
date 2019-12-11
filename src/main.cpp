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

#include "Functions.h"
#include "Scripting.h"
#include "Utils.h"
#include <fstream>

#ifdef LINUX
#include <cstring>
typedef unsigned char *PCHAR;
#endif

//----------------------------------------------------------

void **ppPluginData;
extern void *pAMXFunctions;
logprintf_t logprintf;

// Internal server pointers
CNetGame *pNetGame = 0;
void *pConsole = 0;
RakServer *pRakServer = 0;

eSAMPVersion iVersion = eSAMPVersion::SAMP_VERSION_UNKNOWN;

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

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	ppPluginData = ppData;
	pAMXFunctions = ppPluginData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = reinterpret_cast<logprintf_t>(ppPluginData[PLUGIN_DATA_LOGPRINTF]);
	iVersion = GetServerVersion();

#ifndef _WIN32
	LoadTickCount();
#endif

	CAddress::Initialize(iVersion);
	InstallPreHooks();
	ShowPluginInfo(iVersion);

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

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	static bool bFirst = false;

	if (!bFirst)
	{
		bFirst = true;
		CSAMPFunctions::Initialize();
	}

	return InitScripting(amx);
}

//----------------------------------------------------------
// When a gamemode is over or a filterscript gets unloaded, this
// function gets called. No special actions needed in here.

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	return AMX_ERR_NONE;
}
