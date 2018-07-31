
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

using std::isfinite;

extern void *pAMXFunctions;

subhook_t GetPacketID_hook;

BOOL knifeSync = true;
int lastAnim[1000] = {0};
DWORD lastUpdateTick[1000] = {0};
BOOL blockKeySync[1000] = {0};

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
	for(; *mask; ++mask, ++data, ++pattern) {
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
	for(i = 0; i < size; ++i) {
		if(memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
			return (DWORD)(address + i);
	}
	return 0;
}

///////////////////////////////////////////////////////////////
// Hooks //
///////////////////////////////////////////////////////////////

bool IsPlayerUpdatePacket(unsigned char packetId)
{
	return (
			   packetId == ID_PLAYER_SYNC ||
			   packetId == ID_VEHICLE_SYNC ||
			   packetId == ID_PASSENGER_SYNC ||
			   packetId == ID_SPECTATOR_SYNC ||
			   packetId == ID_AIM_SYNC ||
			   packetId == ID_TRAILER_SYNC
		   );
}

typedef BYTE (*FUNC_GetPacketID)(Packet *p);
BYTE lastWeapon[1000] = {0};
CSyncData lastSyncData[1000];
BOOL syncDataFrozen[1000] = {0};
BYTE fakeHealth[1000] = {0};
BYTE fakeArmour[1000] = {0};
glm::quat* fakeQuat[1000];
BOOL disableSyncBugs = true;
BOOL infiniteAmmo[1000] = {0};

static BYTE HOOK_GetPacketID(Packet *p)
{
	//BYTE packetId = ((FUNC_GetPacketID)subhook_get_trampoline(GetPacketID_hook))(p);
	subhook_remove(GetPacketID_hook);
	BYTE packetId = ((FUNC_GetPacketID)CAddress::FUNC_GetPacketID)(p);
	WORD playerid = p->playerIndex;

	if (packetId == 0xFF) {
		subhook_install(GetPacketID_hook);
		return 0xFF;
	}

	if (IsPlayerUpdatePacket(packetId)) {
		lastUpdateTick[playerid] = GetTickCount();
	}

	if (packetId == ID_PLAYER_SYNC) {
		CSyncData *d = (CSyncData*)(&p->data[1]);

		// NAN stuff = inf loop, no idea why.
		// This prevents it though, so I didn't bother to look too deep into it.
		if (d->vecPosition.IsNan() ||
			d->vecQuaternion.IsNan() ||
			d->vecSurfing.IsNan() ||
			d->vecVelocity.IsNan() ||
			d->fQuaternionAngle != d->fQuaternionAngle)
		{
			subhook_install(GetPacketID_hook);
			return packetId;
		}

		if (disableSyncBugs) {
			// Prevent "ghost shooting" bugs
			if ((d->byteWeapon >= WEAPON_COLT45 && d->byteWeapon <= WEAPON_SNIPER) || d->byteWeapon == WEAPON_MINIGUN)
			{
				switch (d->wAnimIndex) {
					// PED_RUN_*
				case 1222:
				case 1223:
				case 1224:
				case 1225:
				case 1226:
				case 1227:
				case 1228:
				case 1229:
				case 1230:
				case 1231:
				case 1232:
				case 1233:
				case 1234:
				case 1235:
				case 1236:
					// PED_SWAT_RUN
				case 1249:
					// PED_WOMAN_(RUN/WALK)_*
				case 1275:
				case 1276:
				case 1277:
				case 1278:
				case 1279:
				case 1280:
				case 1281:
				case 1282:
				case 1283:
				case 1284:
				case 1285:
				case 1286:
				case 1287:
					// FAT_FATRUN_ARMED
				case 459:
					// MUSCULAR_MUSCLERUN*
				case 908:
				case 909:
					// PED_WEAPON_CROUCH
				case 1274:
					// PED_WALK_PLAYER
				case 1266:
					// PED_SHOT_PARTIAL(_B)
				case 1241:
				case 1242:
					// Baseball bat
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
					// Knife
				case 745:
				case 746:
				case 747:
				case 748:
				case 749:
				case 750:
				case 751:
				case 752:
				case 753:
				case 754:
				case 755:
				case 756:
				case 757:
				case 758:
				case 759:
				case 760:
					// Sword
				case 1545:
				case 1546:
				case 1547:
				case 1548:
				case 1549:
				case 1550:
				case 1551:
				case 1552:
				case 1553:
				case 1554:
					// Fight
				case 471:
				case 472:
				case 473:
				case 474:
				case 477:
				case 478:
				case 479:
				case 480:
				case 481:
				case 482:
				case 483:
				case 484:
				case 485:
				case 486:
				case 487:
				case 488:
				case 489:
				case 490:
				case 491:
				case 492:
				case 493:
				case 494:
				case 495:
				case 496:
				case 497:
				case 498:
				case 499:
				case 500:
				case 501:
				case 502:
				case 503:
				case 504:
				case 505:
				case 506:
				case 507:
				case 1135:
				case 1136:
				case 1137:
				case 1138:
				case 1139:
				case 1140:
				case 1141:
				case 1142:
				case 1143:
				case 1144:
				case 1145:
				case 1146:
				case 1147:
				case 1148:
				case 1149:
				case 1150:
				case 1151:
					// Only remove action key if holding aim
					if (d->wKeys & 128) {
						d->wKeys &= ~1;
					}

					// Remove fire key
					d->wKeys &= ~4;

					// Remove aim key
					d->wKeys &= ~128;

					break;
				}

			}
			else if (d->byteWeapon == WEAPON_SPRAYCAN || d->byteWeapon == WEAPON_FIREEXTINGUISHER || d->byteWeapon == WEAPON_FLAMETHROWER)
			{
				if (d->wAnimIndex < 1160 || d->wAnimIndex > 1167) {
					// Only remove action key if holding aim
					if (d->wKeys & 128) {
						d->wKeys &= ~1;
					}

					// Remove fire key
					d->wKeys &= ~4;

					// Remove aim key
					d->wKeys &= ~128;
				}

			} 
			else if (d->byteWeapon == WEAPON_GRENADE)
			{
				if (d->wAnimIndex < 644 || d->wAnimIndex > 646) {
					d->wKeys &= ~1;
				}
			}
		}

		if (syncDataFrozen[playerid]) {
			memcpy(d, &lastSyncData[playerid], sizeof(CSyncData));
		} else {
			memcpy(&lastSyncData[playerid], d, sizeof(CSyncData));
		}

		if (blockKeySync[playerid]) {
			d->wKeys = 0;
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

		int anim = d->dwAnimationData;
		BOOL animChanged = (lastAnim[playerid] != anim);

		lastAnim[playerid] = anim;

		lastWeapon[playerid] = d->byteWeapon;
	}

	if (packetId == ID_AIM_SYNC) {
		CAimSyncData *d = (CAimSyncData*)(&p->data[1]);

		// Never had an issue with getting crashed here, but... better to check.
		if (d->vecFront.IsNan() || d->vecPosition.IsNan()) {
			subhook_install(GetPacketID_hook);
			return packetId;
		}

		// Fix first-person up/down aim sync
		if (lastWeapon[playerid] == 34 || lastWeapon[playerid] == 35 || lastWeapon[playerid] == 36 || lastWeapon[playerid] == 43) {
			d->fZAim = -d->vecFront.fZ;

			if (d->fZAim > 1.0f) {
				d->fZAim = 1.0f;
			} else if (d->fZAim < -1.0f) {
				d->fZAim = -1.0f;
			}
		}

		if (infiniteAmmo[playerid]) {
			d->byteCameraZoom = 2;
		}
	}

	if (packetId == ID_VEHICLE_SYNC) {
		CVehicleSyncData *d = (CVehicleSyncData*)(&p->data[1]);

		if (fakeHealth[playerid] != 255) {
			d->bytePlayerHealth = fakeHealth[playerid];
		}

		if (fakeArmour[playerid] != 255) {
			d->bytePlayerArmour = fakeArmour[playerid];
		}
	}

	if (packetId == ID_PASSENGER_SYNC) {
		CPassengerSyncData *d = (CPassengerSyncData*)(&p->data[1]);

		if (fakeHealth[playerid] != 255) {
			d->bytePlayerHealth = fakeHealth[playerid];
		}

		if (fakeArmour[playerid] != 255) {
			d->bytePlayerArmour = fakeArmour[playerid];
		}
	}

	subhook_install(GetPacketID_hook);

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
