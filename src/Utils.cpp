#include "Utils.h"
#include "main.h"
#include "Versions.h"

#include "Addresses.h"
#include <string.h>

// Linux GetTickCount
#ifndef _WIN32

#include "sys/time.h"
timeval startTime;
timeval currentTime;

void LoadTickCount()
{
	// Get the starting time
	gettimeofday(&startTime, 0);
}

int GetTickCount()
{
	// Get the time elapsed since the start
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_usec - startTime.tv_usec) / 1000 + 1000 * (currentTime.tv_sec - startTime.tv_sec);
}

#endif

eSAMPVersion GetServerVersion()
{
	// Check server version
	eSAMPVersion version = eSAMPVersion::SAMP_VERSION_UNKNOWN;

	if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03Z)
	{
		version = eSAMPVersion::SAMP_VERSION_03Z;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR2_2)
	{
		version = eSAMPVersion::SAMP_VERSION_03Z_R2_2;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR3)
	{
		version = eSAMPVersion::SAMP_VERSION_03Z_R3;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03ZR4)
	{
		version = eSAMPVersion::SAMP_VERSION_03Z_R4;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_037)
	{
		version = eSAMPVersion::SAMP_VERSION_037;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_037_R2_1)
	{
		version = eSAMPVersion::SAMP_VERSION_037R2;
	}
	else if (logprintf == (logprintf_t)CAddress::FUNC_Logprintf_03DL_R1)
	{
		version = eSAMPVersion::SAMP_VERSION_03DL_R1;
	}

	return version;
}

char* GetServerVersionToStr(eSAMPVersion version)
{
	char* versionString = new char[64];

	switch(version)
	{
		case SAMP_VERSION_UNKNOWN: 
			strcpy(versionString, "Unknown");
			break;

		case SAMP_VERSION_03Z: 
			strcpy(versionString, "0.3z");
			break;

		case SAMP_VERSION_03Z_R2_2: 
			strcpy(versionString, "0.3z R2-2");
			break;

		case SAMP_VERSION_03Z_R3: 
			strcpy(versionString, "0.3z R3");
			break;

		case SAMP_VERSION_03Z_R4: 
			strcpy(versionString, "0.3z R4");
			break;

		case SAMP_VERSION_037RC1: 
			strcpy(versionString, "0.3.7 RC1");
			break;

		case SAMP_VERSION_037: 
			strcpy(versionString, "0.3.7");
			break;

		case SAMP_VERSION_037R2: 
			strcpy(versionString, "0.3.7 R2");
			break;

		case SAMP_VERSION_03DL_R1: 
			strcpy(versionString, "0.3.DL R1");
			break;
		
		default: {
			strcpy(versionString, "Unsupported Version");
		}
	}

	return versionString;
}

void ShowPluginInfo(eSAMPVersion version)
{
	logprintf("");
	logprintf(" ===============================");
	logprintf("   ");	
	logprintf("   < " PROJECT_NAME " - " PROJECT_VERSION " >");
	logprintf("   ");	
	logprintf("   (c) 2008 - Present | YSF Maintainers");	
	logprintf("   (c) 2015 | Oscar \"Slice\" Broman");
	logprintf("   ");
	logprintf("   Server Version: %s", GetServerVersionToStr(version));
	logprintf("   Operating System: " OS_NAME);
	logprintf("   Built on: " __DATE__ " at " __TIME__);
	logprintf("   ");	
	logprintf(" ===============================");
	logprintf("");	
}

bool IsPlayerConnected(int playerid)
{
	return Versions::getNetGame([playerid](auto netGame, auto structs) -> bool {
		if(playerid < 0 || playerid >= 1000) 
			return false;

		return netGame->pPlayerPool->pPlayer[playerid] != 0;
	});
}

bool ValidatePosition(const CVector& position) {
    // Check for NaN and infinity
    if (!std::isfinite(position.fX) || !std::isfinite(position.fY) || !std::isfinite(position.fZ)) {
        return false;
    }
    
    // Check reasonable world bounds (San Andreas map boundaries)
    const float MAX_COORD = 4000.0f;
    const float MIN_COORD = -4000.0f;
    
    if (position.fX < MIN_COORD || position.fX > MAX_COORD ||
        position.fY < MIN_COORD || position.fY > MAX_COORD ||
        position.fZ < -100.0f || position.fZ > 2000.0f) { // Z has different bounds
        return false;
    }
    
    return true;
}

bool ValidatePlayerID(int playerid) {
    if (playerid < 0 || playerid >= 1000) {
        return false;
    }
    
    return true;
}

bool ValidateWeaponID(int weaponid) {
    // Weapon IDs range from 0 to 46
    // 0 = Fist/Unarmed
    // 1-18 = Melee weapons and thrown weapons
    // 22-46 = Firearms and special weapons
    // 19, 20, 21 are invalid weapon IDs
    if (weaponid < 0 || weaponid > 46) {
        return false;
    }
    
    // Check for invalid weapon IDs in the valid range
    if (weaponid >= 19 && weaponid <= 21) {
        return false;
    }
    
    return true;
}

bool ValidateVehicleID(int vehicleid) {
	// Vehicle IDs range from 0 to 1999
	if (vehicleid < 0 || vehicleid >= 2000) {
		return false;
	}
	
	return true;
}