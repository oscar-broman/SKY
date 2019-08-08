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

#include "Scripting.h"

#include <raknet/BitStream.h>
#include "RPCs.h"
#include "Utils.h"
#include "main.h"
#include "Functions.h"
#include "Hooks.h"
#include "Addresses.h"
#include <cmath>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Yes - BOTH string versions...
#include <strsafe.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
//	#include <sys/times.h>
#include <algorithm>
#endif

#include <string.h>
#include <stdio.h>

// extern
typedef cell AMX_NATIVE_CALL (* AMX_Function_t)(AMX *amx, cell *params);

//----------------------------------------------------
const float MATH_PI = 3.14159265359f;
const float RAD_TO_DEG = 180.0f / MATH_PI;
const float DEG_TO_RAD = 1.0f / RAD_TO_DEG;

template <class Type>
struct Natives : Type
{
	// native SpawnPlayerForWorld(playerid);
	static cell AMX_NATIVE_CALL SpawnPlayerForWorld( AMX* amx, cell* params )
	{
		CHECK_PARAMS(1, "SpawnPlayerForWorld");

		int playerid = (int)params[1];

		CSAMPFunctions::SpawnPlayer(playerid);

		return 1;
	}

	// native SetLastAnimationData(playerid, data)
	static cell AMX_NATIVE_CALL SetLastAnimationData( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetLastAnimationData");

		int playerid = (int)params[1];
		int data = (int)params[2];

		if (playerid < 0 || playerid >= 1000)
			return 0;

		lastSyncData[playerid].dwAnimationData = data;

		return 1;
	}

	// native SendLastSyncData(playerid, toplayerid, animation = 0)
	static cell AMX_NATIVE_CALL SendLastSyncData( AMX* amx, cell* params )
	{
		CHECK_PARAMS(3, "SendLastSyncData");

		int playerid = (int)params[1];
		int toplayerid = (int)params[2];
		int animation = (int)params[3];
		BYTE ps = ID_PLAYER_SYNC;
		CSyncData* d = &lastSyncData[playerid];

		RakNet::BitStream bs;
		bs.Write((BYTE)ID_PLAYER_SYNC);
		bs.Write((WORD)playerid);

		if (d->wUDAnalog) {
			bs.Write(true);
			bs.Write((WORD)d->wUDAnalog);
		} else {
			bs.Write(false);
		}

		if (d->wLRAnalog) {
			bs.Write(true);
			bs.Write((WORD)d->wLRAnalog);
		} else {
			bs.Write(false);
		}

		bs.Write((WORD)d->wKeys);

		bs.Write(d->vecPosition.fX);
		bs.Write(d->vecPosition.fY);
		bs.Write(d->vecPosition.fZ);

		if (fakeQuat[playerid] != NULL) {
			bs.Write((bool)(fakeQuat[playerid]->w<0.0f));
			bs.Write((bool)(fakeQuat[playerid]->x<0.0f));
			bs.Write((bool)(fakeQuat[playerid]->y<0.0f));
			bs.Write((bool)(fakeQuat[playerid]->z<0.0f));
			bs.Write((unsigned short)(fabs(fakeQuat[playerid]->x)*65535.0));
			bs.Write((unsigned short)(fabs(fakeQuat[playerid]->y)*65535.0));
			bs.Write((unsigned short)(fabs(fakeQuat[playerid]->z)*65535.0));
		} else {
			bs.Write((bool)(d->fQuaternion[0]<0.0f));
			bs.Write((bool)(d->fQuaternion[1]<0.0f));
			bs.Write((bool)(d->fQuaternion[2]<0.0f));
			bs.Write((bool)(d->fQuaternion[3]<0.0f));
			bs.Write((unsigned short)(fabs(d->fQuaternion[1])*65535.0));
			bs.Write((unsigned short)(fabs(d->fQuaternion[2])*65535.0));
			bs.Write((unsigned short)(fabs(d->fQuaternion[3])*65535.0));
		}

		BYTE health, armour;

		if (fakeHealth[playerid] != 255) {
			health = fakeHealth[playerid];
		} else {
			health = d->byteHealth;
		}

		if (fakeArmour[playerid] != 255) {
			armour = fakeArmour[playerid];
		} else {
			armour = d->byteArmour;
		}

		if (health >= 100) {
			health = 0xF;
		} else {
			health /= 7;
		}

		if (armour >= 100) {
			armour = 0xF;
		} else {
			armour /= 7;
		}

		bs.Write((BYTE)((health << 4) | (armour)));

		bs.Write(d->byteWeapon);
		bs.Write(d->byteSpecialAction);

		// Make them appear standing still if paused
		if (GetTickCount() - lastUpdateTick[playerid] > 2000) {
			bs.WriteVector(0.0f, 0.0f, 0.0f);
		} else {
			bs.WriteVector(d->vecVelocity.fX, d->vecVelocity.fY, d->vecVelocity.fZ);
		}

		if (d->wSurfingInfo) {
			bs.Write(true);

			bs.Write(d->wSurfingInfo);
			bs.Write(d->vecSurfing.fX);
			bs.Write(d->vecSurfing.fY);
			bs.Write(d->vecSurfing.fZ);
		} else {
			bs.Write(false);
		}

		// Animations are only sent when they are changed
		if (animation) {
			bs.Write(true);
			bs.Write(animation);
		} else {
			bs.Write(false);
		}

		CSAMPFunctions::Send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, CSAMPFunctions::GetPlayerIDFromIndex(toplayerid), false);

		return 1;
	}

	// native SetFakeArmour(playerid, health);
	static cell AMX_NATIVE_CALL SetFakeArmour( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetFakeArmour");

		int playerid = (int)params[1];
		BYTE armour = (BYTE)params[2];

		fakeArmour[playerid] = armour;

		return 1;
	}

	// native SetFakeHealth(playerid, health);
	static cell AMX_NATIVE_CALL SetFakeHealth( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetFakeHealth");

		int playerid = (int)params[1];
		BYTE health = (BYTE)params[2];

		fakeHealth[playerid] = health;

		return 1;
	}

	// native SetFakeFacingAngle(playerid, Float:angle)
	static cell AMX_NATIVE_CALL SetFakeFacingAngle( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetFakeFacingAngle");

		int playerid = (int)params[1];

		if ((int)params[2] == 0x7FFFFFFF) {
			fakeQuat[playerid] = NULL;
		} else {
			glm::vec3 vec = glm::vec3(0.0f, 0.0f, 360.0f - amx_ctof(params[2]));

			fakeQuat[playerid] = new glm::quat(vec * DEG_TO_RAD);
		}


		return 1;
	}

	// native SetKnifeSync(toggle);
	static cell AMX_NATIVE_CALL SetKnifeSync( AMX* amx, cell* params )
	{
		CHECK_PARAMS(1, "SetKnifeSync");

		knifeSync = (BOOL)params[1];

		return 1;
	}

	// native SetDisableSyncBugs(toggle);
	static cell AMX_NATIVE_CALL SetDisableSyncBugs( AMX* amx, cell* params )
	{
		CHECK_PARAMS(1, "SetDisableSyncBugs");

		disableSyncBugs = (BOOL)params[1];

		return 1;
	}

	// native SetInfiniteAmmoSync(playerid, toggle)
	static cell AMX_NATIVE_CALL SetInfiniteAmmoSync( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetInfiniteAmmoSync");

		int playerid = (int)params[1];
		BOOL toggle = (BOOL)params[2];

		infiniteAmmo[playerid] = toggle;

		return 1;
	}
	// native SetKeySyncBlocked(playerid, toggle)
	static cell AMX_NATIVE_CALL SetKeySyncBlocked( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "SetKeySyncBlocked");

		int playerid = (int)params[1];
		BOOL toggle = (BOOL)params[2];

		blockKeySync[playerid] = toggle;

		return 1;
	}

	// native ClearAnimationsForPlayer(playerid, forplayerid)
	static cell AMX_NATIVE_CALL ClearAnimationsForPlayer( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "ClearAnimationsForPlayer");

		int playerid = (int)params[1];
		int forplayerid = (int)params[2];

		RakNet::BitStream bs;
		bs.Write((WORD)playerid);

		CSAMPFunctions::RPC(&RPC_ClearAnimations, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, CSAMPFunctions::GetPlayerIDFromIndex(forplayerid), false, false);

		return 1;
	}

	// native SendDeath(playerid);
	static cell AMX_NATIVE_CALL SendDeath( AMX* amx, cell* params )
	{
		CHECK_PARAMS(1, "SendDeath");

		int playerid = (int)params[1];

		CPlayer *pPlayer = pNetGame->pPlayerPool->pPlayer[playerid];

		pPlayer->byteState = PLAYER_STATE_WASTED;


		RakNet::BitStream bs;
		bs.Write((WORD)playerid);

		CSAMPFunctions::RPC(&RPC_DeathBroadcast, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, CSAMPFunctions::GetPlayerIDFromIndex(playerid), true, false);

		return 1;
	}

	// native FreezeSyncData(playerid, bool:toggle)
	static cell AMX_NATIVE_CALL FreezeSyncData( AMX* amx, cell* params )
	{
		CHECK_PARAMS(2, "FreezeSyncData");

		int playerid = (int)params[1];
		BOOL toggle = (BOOL)params[2];

		lastSyncData[playerid].vecVelocity = CVector();
		lastSyncData[playerid].byteSpecialAction = 0;
		lastSyncData[playerid].wKeys = 0;
		lastSyncData[playerid].wUDAnalog = 0;
		lastSyncData[playerid].wLRAnalog = 0;

		syncDataFrozen[playerid] = toggle;

		return 1;
	}

	// native TextDrawSetPosition(Text:text, Float:fX, Float:fY)
	static cell AMX_NATIVE_CALL TextDrawSetPosition( AMX* amx, cell* params )
	{
		CHECK_PARAMS(3, "TextDrawSetPosition");
		
		int textdrawid = (int)params[1];
		if(textdrawid < 0 || textdrawid >= MAX_TEXT_DRAWS) return 0;
		
		if(!pNetGame || !pNetGame->pTextDrawPool || !pNetGame->pTextDrawPool->bSlotState[textdrawid]) return 0;
		CTextdraw *pTD = pNetGame->pTextDrawPool->TextDraw[textdrawid];

		pTD->vecPos.fX = amx_ctof(params[2]);
		pTD->vecPos.fY = amx_ctof(params[3]);
		return 1;
	}

	// native PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:fX, Float:fY)
	static cell AMX_NATIVE_CALL PlayerTextDrawSetPosition( AMX* amx, cell* params )
	{
		CHECK_PARAMS(4, "PlayerTextDrawSetPosition");
		
		int playerid = (int)params[1];
		int textdrawid = (int)params[2];

		if(textdrawid < 0 || textdrawid >= MAX_PLAYER_TEXT_DRAWS) return 0;
		if(playerid < 0 || playerid >= 1000) return 0;
		if(!pNetGame->pPlayerPool->pPlayer[playerid]) return 0;
		if(!pNetGame->pPlayerPool->pPlayer[playerid]->pTextdraw->bSlotState[textdrawid]) return 0;

		CTextdraw *pTD = pNetGame->pPlayerPool->pPlayer[playerid]->pTextdraw->TextDraw[textdrawid];

		pTD->vecPos.fX = amx_ctof(params[3]);
		pTD->vecPos.fY = amx_ctof(params[4]);
		return 1;
	}

	// native TextDrawSetStringForPlayer(Text:text, playerid, const string[])
	static cell AMX_NATIVE_CALL TextDrawSetStrForPlayer( AMX* amx, cell* params )
	{
		CHECK_PARAMS(3, "TextDrawSetStrForPlayer");

		int textdrawid = (int)params[1];
		if(textdrawid < 0 || textdrawid >= MAX_TEXT_DRAWS) return 0;

		int playerid = (int)params[2];
		char* text;
		amx_StrParam(amx, params[3], text);
		unsigned short len = (unsigned short)strlen(text);

		RakNet::BitStream bs;
		bs.Write((WORD)textdrawid);
		bs.Write((unsigned short)len);
		bs.Write(text, len + 1);

		CSAMPFunctions::RPC(&RPC_ScrEditTextDraw, &bs, HIGH_PRIORITY, RELIABLE, 0, CSAMPFunctions::GetPlayerIDFromIndex(playerid), false, false);

		return 1;
	}
};

template <class Type>
AMX_NATIVE_INFO *get_natives()
{
    static AMX_NATIVE_INFO native_list[] = {
		{ "SpawnPlayerForWorld",		Natives<Type>::SpawnPlayerForWorld },
		{ "SetFakeHealth",				Natives<Type>::SetFakeHealth },
		{ "SetFakeArmour",				Natives<Type>::SetFakeArmour },
		{ "SetFakeFacingAngle",			Natives<Type>::SetFakeFacingAngle },
		{ "FreezeSyncData",				Natives<Type>::FreezeSyncData },
		{ "SetKnifeSync",				Natives<Type>::SetKnifeSync },
		{ "SendDeath",					Natives<Type>::SendDeath },
		{ "SetLastAnimationData",		Natives<Type>::SetLastAnimationData },
		{ "SendLastSyncData",			Natives<Type>::SendLastSyncData },
		{ "SetDisableSyncBugs",			Natives<Type>::SetDisableSyncBugs },
		{ "ClearAnimationsForPlayer",	Natives<Type>::ClearAnimationsForPlayer },
		{ "SetKeySyncBlocked",			Natives<Type>::SetKeySyncBlocked },
		{ "SetInfiniteAmmoSync",		Natives<Type>::SetInfiniteAmmoSync },
		{ "TextDrawSetPosition",		Natives<Type>::TextDrawSetPosition },
		{ "PlayerTextDrawSetPosition",	Natives<Type>::PlayerTextDrawSetPosition },
		{ "TextDrawSetStrForPlayer",	Natives<Type>::TextDrawSetStrForPlayer },
		{ 0,							0 }
    };
    return native_list;
}

int InitScripting(AMX *amx, int iVersion)
{
	if(iVersion == eSAMPVersion::SAMP_VERSION_03DL_R1) 
		return amx_Register(amx, get_natives<with_dl>(), -1);
	else 
		return amx_Register(amx, get_natives<without_dl>(), -1);
}
