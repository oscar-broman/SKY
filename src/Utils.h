#pragma once

#include "main.h"
#include "CVector.h"

// Linux
#ifndef _WIN32
int GetTickCount();
void LoadTickCount();
#endif

eSAMPVersion GetServerVersion();
char* GetServerVersionToStr(eSAMPVersion version);
void ShowPluginInfo(eSAMPVersion version);
bool IsPlayerConnected(int playerid);
bool ValidatePlayerID(int playerid);
bool ValidateWeaponID(int weaponid);
bool ValidateVehicleID(int vehicleid);
bool ValidatePosition(const CVector& position);