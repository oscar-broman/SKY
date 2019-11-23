#pragma once

#include "main.h"

// Linux
#ifndef _WIN32
int GetTickCount();
void LoadTickCount();
#endif

int GetServerVersion();