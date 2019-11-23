/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: Utils.cpp
	- Author(s): OrMisicL

  =========================================*/

#include "Utils.h"
#include "main.h"

#include "Addresses.h"

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

int GetServerVersion()
{
	// Check server version
	int version = eSAMPVersion::SAMP_VERSION_UNKNOWN;

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