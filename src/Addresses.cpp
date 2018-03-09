#include "Addresses.h"

#include "Hooks.h"
#include "main.h"

#include <sdk/plugin.h>

#ifdef _WIN32
DWORD CAddress::FUNC_Logprintf_03Z = 0x00486CB0;
DWORD CAddress::FUNC_Logprintf_03ZR2_2 = 0x00487310;
DWORD CAddress::FUNC_Logprintf_03ZR3 = 0x00487460;
DWORD CAddress::FUNC_Logprintf_03ZR4 = 0x004875F0;
DWORD CAddress::FUNC_Logprintf_037 = 0x0048A0B0;
DWORD CAddress::FUNC_Logprintf_037_R2_1 = 0x0048C8D0;
DWORD CAddress::FUNC_Logprintf_03DL_R1 = 0x00491FA0;
#else
DWORD CAddress::FUNC_Logprintf_03Z = 0x080A7440;
DWORD CAddress::FUNC_Logprintf_03ZR2_2 = 0x080A77D0;
DWORD CAddress::FUNC_Logprintf_03ZR3 = 0x080A78E0;
DWORD CAddress::FUNC_Logprintf_03ZR4 = 0x80A7A90;
DWORD CAddress::FUNC_Logprintf_037 = 0x080A9000;
DWORD CAddress::FUNC_Logprintf_037_R2_1 = 0x080A91D0;
#endif

// Pointers
DWORD CAddress::VAR_ppNetGame = NULL;
DWORD CAddress::VAR_ppConsole = NULL;
DWORD CAddress::VAR_ppRakServer = NULL;

// Variables
DWORD CAddress::VAR_pRestartWaitTime = NULL;

// Functions
DWORD CAddress::FUNC_CConsole__AddStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__FindVariable = NULL;
DWORD CAddress::FUNC_CConsole__SendRules = NULL;
DWORD CAddress::FUNC_CConsole__Execute = NULL;

DWORD CAddress::FUNC_CFilterscripts__LoadFilterscript = NULL;
DWORD CAddress::FUNC_CFilterscripts__UnLoadFilterscript = NULL;
DWORD CAddress::FUNC_ContainsInvalidChars = NULL;
DWORD CAddress::FUNC_GetPacketID = NULL;

DWORD CAddress::FUNC_CPlayer__SpawnForWorld = NULL;
DWORD CAddress::FUNC_ProcessQueryPacket = NULL;
DWORD CAddress::FUNC_Packet_WeaponsUpdate = NULL;
DWORD CAddress::FUNC_format_amxstring = NULL;

// Others
DWORD CAddress::ADDR_CNetGame_GMX_GangZoneDelete = NULL;
DWORD CAddress::ADDR_CNetGame_GMX_PckupDelete = NULL;

void CAddress::Initialize()
{
	// Thx for Whitetiger
#ifdef _WIN32
	VAR_pRestartWaitTime						= 0x0051C188;

	FUNC_CConsole__AddStringVariable			= 0x00491C40;
	FUNC_CConsole__FindVariable					= 0x00490AA0;
	FUNC_CConsole__SendRules					= 0x00490770;
	FUNC_CConsole__Execute						= 0x00490CE0;

	FUNC_CFilterscripts__LoadFilterscript		= 0x0046A9D0;
	FUNC_CFilterscripts__UnLoadFilterscript		= 0x0046D1C0;

	FUNC_ContainsInvalidChars = 0x00468EE0;

	/*todo:
	FUNC_GetPacketID =							FindPattern("\x8B\x44\x24\x04\x85\xC0\x75\x03\x0C\xFF\xC3", "xxxxxxx???x");
	*/
	
	FUNC_CPlayer__SpawnForWorld					= 0x00487730;
	FUNC_ProcessQueryPacket						= 0x0049E480;
	FUNC_Packet_WeaponsUpdate					= 0x00495940;

	/*todo:
	//ADDR_CNetGame_GMX_GangZoneDelete =			FindPattern("\x83\xC4\x04\x89\x5E\x24", "xxxxxx") - 0x8;
	//ADDR_CNetGame_GMX_PckupDelete =				FindPattern("\x83\xC4\x04\x89\x5E\x10", "xxxxxx") - 0x8;
	*/

	FUNC_format_amxstring						= 0x0046FD00;


	DWORD iRealProcessNetworkPacket = FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x5C", "xxx????xxxxxxxxxxxxxxxxx");

	DWORD iSocketLayerSendTo = FindPattern("\x83\xEC\x10\x55\x8B\x6C\x24\x18\x83\xFD\xFF", "xxxxxxxxxxx");

	switch(serverVersion) {
	case SAMP_VERSION_03Z: {
		//ADDR_RECEIVE_HOOKPOS =						0x458B80;
		break;
	}
	case SAMP_VERSION_03Z_R4: {
		//ADDR_RECEIVE_HOOKPOS =						0x458A20;
		break;
	}
	}
#else

	// Thx for Mellnik
	VAR_pRestartWaitTime						= 0x08166610;

	FUNC_CConsole__AddStringVariable			= 0x080A8E00;

	FUNC_CConsole__FindVariable					= 0x080A8250;
	FUNC_CConsole__SendRules					= 0x080A7CB0;
	FUNC_CConsole__Execute						= 0x080A8330;

	FUNC_CFilterscripts__LoadFilterscript		= 0x080A9980;
	FUNC_CFilterscripts__UnLoadFilterscript		= 0x080A9DB0;


	FUNC_ContainsInvalidChars					= 0x080E1220;

	FUNC_GetPacketID =							FindPattern("\x55\xB8\x00\x00\x00\x00\x89\xE5\x8B\x55\x00\x85\xD2", "xx????xxxx?xx");

	FUNC_CPlayer__SpawnForWorld					= 0x080D7870;
	FUNC_ProcessQueryPacket						= 0x080DE960;
	FUNC_Packet_WeaponsUpdate					= 0x080B52A0;

	ADDR_CNetGame_GMX_GangZoneDelete =			NULL;
	ADDR_CNetGame_GMX_PckupDelete =				NULL;
	switch(serverVersion) {
	case SAMP_VERSION_03Z: {
		VAR_pRestartWaitTime =						0x8150130;
		break;
	}
	case SAMP_VERSION_03Z_R2_2: {
		VAR_pRestartWaitTime =						0x8150B60;
		break;
	}
	case SAMP_VERSION_03Z_R3: {
		VAR_pRestartWaitTime =						0x81512F0;
		break;
	}
	case SAMP_VERSION_03Z_R4: {
		VAR_pRestartWaitTime =						0x81514E0; // 12.0
		FUNC_Packet_WeaponsUpdate =					0x80AAC90;
		FUNC_ProcessQueryPacket =					0x080D0EB0;
		FUNC_CConsole__FindVariable =				0x809EA60;
		FUNC_CConsole__SendRules =					0x809E4C0;
		FUNC_CConsole__Execute =					0x809EB40;
		break;
	}
	case SAMP_VERSION_037RC1: {
		break;
	}
	}
#endif

	logprintf("VAR_pRestartWaitTime: %X", VAR_pRestartWaitTime);

	logprintf("FUNC_CConsole__AddStringVariable: %X", FUNC_CConsole__AddStringVariable);

	logprintf("FUNC_CConsole__FindVariable: %X", FUNC_CConsole__FindVariable);
	logprintf("FUNC_CConsole__SendRules: %X", FUNC_CConsole__SendRules);
	logprintf("FUNC_CConsole__Execute: %X", FUNC_CConsole__Execute);


	logprintf("FUNC_CFilterscripts__LoadFilterscript: %X", FUNC_CFilterscripts__LoadFilterscript);
	logprintf("FUNC_CFilterscripts__UnLoadFilterscript: %X", FUNC_CFilterscripts__UnLoadFilterscript);

	logprintf("FUNC_ContainsInvalidChars: %X", FUNC_ContainsInvalidChars);

	logprintf("FUNC_CPlayer__SpawnForWorld: %X", FUNC_CPlayer__SpawnForWorld);

	logprintf("FUNC_ProcessQueryPacket: %X", FUNC_ProcessQueryPacket);
	logprintf("FUNC_Packet_WeaponsUpdate: %X", FUNC_Packet_WeaponsUpdate);
	logprintf("FUNC_format_amxstring: %X", FUNC_format_amxstring);


	// Unlock restart wait time
	if (VAR_pRestartWaitTime)
		Unlock((void*)VAR_pRestartWaitTime, 4);

#ifdef NEW_PICKUP_SYSTEM
	// Disable PickupPool deletion at GMX
	//Unlock((void*)ADDR_CNetGame_GMX_PckupDelete, 2); // jz      short loc_489DC8 -> change to jnz      short loc_489DC8
	//*(BYTE*)(ADDR_CNetGame_GMX_PckupDelete) = 0x75;	// jnz
#endif
}
