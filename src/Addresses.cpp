#include "Addresses.h"

#include "Hooks.h"
#include "main.h"

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
DWORD CAddress::FUNC_Logprintf_03DL_R1 = 0x080B1CA0;
#endif

DWORD CAddress::FUNC_CPlayer__SpawnForWorld = 0;

void CAddress::Initialize(eSAMPVersion iVersion)
{
#ifdef _WIN32
	if (iVersion != SAMP_VERSION_03DL_R1)
	{
		FUNC_CPlayer__SpawnForWorld = FindPattern("\x56\x8B\xF1\x8B\x86\x3B\x26\x00\x00\x85\xC0\x0F\x84", "xxxxx????xxxx");
	}
	else
	{
		FUNC_CPlayer__SpawnForWorld = 0x00487730;
	}
#else
	if (iVersion != SAMP_VERSION_03DL_R1)
	{
		FUNC_CPlayer__SpawnForWorld = FindPattern("\x55\x89\xE5\x56\x53\x83\xEC\x00\x8B\x75\x00\xA1\x00\x00\x00\x00", "xxxxxxx?xx?x????");
	}
	else
	{
		FUNC_CPlayer__SpawnForWorld = 0x080D7870;
	}
#endif
}
