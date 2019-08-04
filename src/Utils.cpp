/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: Utils.cpp
	- Author(s): OrMisicL

  =========================================*/

#include "Utils.h"

#include "CVector.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

#include <plugincommon.h>
#include <amx/amx.h>

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

float GetDistance3D(CVector *vecPosition, CVector *_vecPosition)
{
	// Get the distance between the two vectors
	float fSX = (_vecPosition->fX - vecPosition->fX) * (_vecPosition->fX - vecPosition->fX);
	float fSY = (_vecPosition->fY - vecPosition->fY) * (_vecPosition->fY - vecPosition->fY);
	float fSZ = (_vecPosition->fZ - vecPosition->fZ) * (_vecPosition->fZ - vecPosition->fZ);
	return ((float)sqrt(fSX + fSY + fSZ));
}

BYTE CUtils::GetWeaponSlot(BYTE weaponid)
{
	BYTE result; // eax@2

	switch (weaponid) {
	case 0:
	case 1:
		result = 0;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		result = 1;
		break;
	case 22:
	case 23:
	case 24:
		result = 2;
		break;
	case 25:
	case 26:
	case 27:
		result = 3;
		break;
	case 28:
	case 29:
	case 32:
		result = 4;
		break;
	case 30:
	case 31:
		result = 5;
		break;
	case 33:
	case 34:
		result = 6;
		break;
	case 35:
	case 36:
	case 37:
	case 38:
		result = 7;
		break;
	case 16:
	case 17:
	case 18:
	case 39:
		result = 8;
		break;
	case 41:
	case 42:
	case 43:
		result = 9;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		result = 10;
		break;
	case 44:
	case 45:
	case 46:
		result = 11;
		break;
	case 40:
		result = 12;
		break;
	default:
		result = -1;
		break;
	}
	return result;
}

// Created by: https://github.com/Zeex/sampgdk/blob/master/plugins/unlimitedfs/unlimitedfs.cpp
std::string GetServerCfgOption(const std::string &option)
{
	std::string name, value;
	std::string line;
	std::fstream server_cfg("server.cfg");
	if (server_cfg) {
		while (std::getline(server_cfg, line)) {
			std::stringstream ss(line);
			ss >> name;
			if (name == option) {
				ss >> value;
				break;
			}
		}
	}
	return "0";
}
