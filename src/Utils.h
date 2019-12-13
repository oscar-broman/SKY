#pragma once

#include "main.h"

// Linux
#ifndef _WIN32
int GetTickCount();
void LoadTickCount();
#endif

eSAMPVersion GetServerVersion();
char* GetServerVersionToStr(eSAMPVersion version);
void ShowPluginInfo(eSAMPVersion version);
bool IsPlayerConnected(int playerid);