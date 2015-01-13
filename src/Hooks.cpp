
/*  
 *  Version: MPL 1.1
 *  
 *  The contents of this file are subject to the Mozilla Public License Version 
 *  1.1 (the "License"); you may not use this file except in compliance with 
 *  the License. You may obtain a copy of the License at 
 *  http://www.mozilla.org/MPL/
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 *  
 *  The Original Code is the YSI 2.0 SA:MP plugin.
 *  
 *  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
 *  Portions created by the Initial Developer are Copyright (C) 2008
 *  the Initial Developer. All Rights Reserved.
 *  
 *  Contributor(s):
 *  
 *  Peter Beverloo
 *  Marcus Bauer
 *  MaVe;
 *  Sammy91
 *  Incognito
 *  
 *  Special Thanks to:
 *  
 *  SA:MP Team past, present and future
 */
#include "Hooks.h"

#include "Addresses.h"
#include "RPCs.h"

#include "main.h"
#include "Utils.h"
#include "Scripting.h"
#include "Structs.h"
#include "Functions.h"
#include <cmath>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	//#define VC_EXTRALEAN
	#include <Windows.h>
	#include <Psapi.h>
#else
	#include <stdio.h>
	#include <sys/mman.h>
	#include <limits.h>
	#include <string.h>
	#include <algorithm>
	#include <unistd.h>
	#include <cstdarg>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>

	#define INVALID_SOCKET -1
#endif

#include "subhook/subhook.h"
#include <sdk/plugin.h>

#ifndef PAGESIZE
	#define PAGESIZE (4096)
#endif

extern void *pAMXFunctions;

subhook_t GetPacketID_hook;

BOOL knifeSync = true;
BOOL distanceBasedStreamRate = true;
int lastAnim[1000] = {0};
DWORD lastUpdateTick[1000] = {0};

// Y_Less - original YSF
bool Unlock(void *address, size_t len)
{
	#ifdef _WIN32
		DWORD
			oldp;
		// Shut up the warnings :D
		return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
	#else
		size_t
			iPageSize = getpagesize(),
			iAddr = ((reinterpret_cast <uint32_t>(address) / iPageSize) * iPageSize);
		return !mprotect(reinterpret_cast <void*>(iAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
	#endif
}

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for(; *mask; ++mask, ++data, ++pattern)
	{
		if(*mask == 'x' && *data != *pattern)
			return false;
	}
	return (*mask) == NULL;
}

DWORD FindPattern(char *pattern, char *mask)
{
	DWORD i;
	DWORD size;
	DWORD address;
#ifdef _WIN32
	MODULEINFO info = { 0 };

	address = (DWORD)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (DWORD)info.SizeOfImage;
#else
	address = 0x804b480; // around the elf base
	size = 0x8128B80 - address;
#endif
	for(i = 0; i < size; ++i)
	{
		if(memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
			return (DWORD)(address + i);
	}
	return 0;
}

///////////////////////////////////////////////////////////////
// Hooks //
///////////////////////////////////////////////////////////////

// Original GetPacketID function
BYTE GetPacketID(Packet *p)
{
	if (p == 0) return 255;

	if ((unsigned char)p->data[0] == 36) 
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else return (unsigned char)p->data[0];
}

bool IsPlayerUpdatePacket(unsigned char packetId)
{
	switch (packetId)
	{
	case ID_PLAYER_SYNC:
	case ID_VEHICLE_SYNC:
	case ID_PASSENGER_SYNC:
	case ID_SPECTATOR_SYNC:
	case ID_AIM_SYNC:
	case ID_TRAILER_SYNC:
		return true;
	default:
		return false;
	}
	return false;
}

typedef BYTE (*FUNC_GetPacketID)(Packet *p);
BYTE lastWeapon[1000] = {0};
CSyncData lastSyncData[1000];
BOOL syncDataFrozen[1000] = {0};
BYTE fakeHealth[1000] = {0};
BYTE fakeArmour[1000] = {0};
glm::quat* fakeQuat[1000];

static BYTE HOOK_GetPacketID(Packet *p)
{
	BYTE packetId = GetPacketID(p);
	WORD playerid = p->playerIndex;

	if (IsPlayerUpdatePacket(packetId)) {
		lastUpdateTick[playerid] = GetTickCount();
	}

	if (packetId == ID_PLAYER_SYNC)
	{
		CSyncData *d = (CSyncData*)(&p->data[1]);

		if (syncDataFrozen[playerid]) {
			memcpy(d, &lastSyncData[playerid], sizeof(CSyncData));
		} else {
			memcpy(&lastSyncData[playerid], d, sizeof(CSyncData));
		}

		if (fakeHealth[playerid] != 255) {
			d->byteHealth = fakeHealth[playerid];
		}

		if (fakeArmour[playerid] != 255) {
			d->byteArmour = fakeArmour[playerid];
		}

		if (fakeQuat[playerid] != NULL) {
			d->fQuaternionAngle = fakeQuat[playerid]->w;
			d->vecQuaternion.fX = fakeQuat[playerid]->x;
			d->vecQuaternion.fY = fakeQuat[playerid]->y;
			d->vecQuaternion.fZ = fakeQuat[playerid]->z;
		}

		if (d->byteWeapon == 44 || d->byteWeapon == 45) {
			d->wKeys &= ~4;
		} else if (d->byteWeapon == 4 && knifeSync == false) {
			d->wKeys &= ~128;
		}

		int anim = d->iAnimationId;
		BOOL animChanged = (lastAnim[playerid] != anim);

		lastAnim[playerid] = anim;

		lastWeapon[playerid] = d->byteWeapon;
	}

	if (packetId == ID_AIM_SYNC)
	{
		// Fix first-person up/down aim sync
		if (lastWeapon[playerid] == 34 || lastWeapon[playerid] == 35 || lastWeapon[playerid] == 36 || lastWeapon[playerid] == 43) {
			CAimSyncData *d = (CAimSyncData*)(&p->data[1]);

			d->fZAim = -d->vecFront.fZ;
		}

	}

	if (packetId == ID_VEHICLE_SYNC)
	{
		CVehicleSyncData *d = (CVehicleSyncData*)(&p->data[1]);

		if (fakeHealth[playerid] != 255) {
			d->bytePlayerHealth = fakeHealth[playerid];
		}

		if (fakeArmour[playerid] != 255) {
			d->bytePlayerArmour = fakeArmour[playerid];
		}
	}

	if (packetId == ID_PASSENGER_SYNC)
	{
		CPassengerSyncData *d = (CPassengerSyncData*)(&p->data[1]);

		if (fakeHealth[playerid] != 255) {
			d->bytePlayerHealth = fakeHealth[playerid];
		}

		if (fakeArmour[playerid] != 255) {
			d->bytePlayerArmour = fakeArmour[playerid];
		}	
	}

	return packetId;
}

//----------------------------------------------------

void InstallPreHooks()
{
	memset(&fakeHealth, 255, sizeof(fakeHealth));
	memset(&fakeArmour, 255, sizeof(fakeArmour));

	for (int i = 0; i < 1000; i++) {
		fakeQuat[i] = NULL;
	}

	if (!serverVersion) {
		return;
	}

	GetPacketID_hook = subhook_new((void*)CAddress::FUNC_GetPacketID, (void*)HOOK_GetPacketID);
	subhook_install(GetPacketID_hook);
}
