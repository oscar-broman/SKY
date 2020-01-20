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

#include "Addresses.h"
#include "Functions.h"
#include "Hooks.h"
#include "RPCs.h"
#include "Utils.h"
#include "Versions.h"
#include "Player.h"
#include "Global.h"
#include "main.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <limits>
#include <raknet/BitStream.h>

#ifdef _WIN32
#include <windows.h>
// Yes - BOTH string versions...
#include <strsafe.h>
#else
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
//	#include <sys/times.h>
#include <algorithm>
#endif

#include <stdio.h>
#include <string.h>

// extern
typedef cell AMX_NATIVE_CALL (*AMX_Function_t)(AMX *amx, cell *params);

//----------------------------------------------------
const float MATH_PI = 3.14159265359f;
const float RAD_TO_DEG = 180.0f / MATH_PI;
const float DEG_TO_RAD = 1.0f / RAD_TO_DEG;

// native SpawnPlayerForWorld(playerid);
static cell AMX_NATIVE_CALL SpawnPlayerForWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SpawnPlayerForWorld");

	int playerid = (int)params[1];

	if(!IsPlayerConnected(playerid)) 
		return 0;

	CSAMPFunctions::SpawnPlayer(playerid);

	return 1;
}

// native SetLastAnimationData(playerid, data)
static cell AMX_NATIVE_CALL SetLastAnimationData(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetLastAnimationData");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		using Structs = decltype(structs);

		int playerid = (int)params[1];
		int animation = (int)params[2];

		if(!IsPlayerConnected(playerid)) 
			return 0;

		auto *d = &Player::getLastSyncData<Structs>(playerid);
		d->dwAnimationData = animation;

		return 1;
	});
}

// native SendLastSyncData(playerid, toplayerid, animation = 0)
static cell AMX_NATIVE_CALL SendLastSyncData(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "SendLastSyncData");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		using Structs = decltype(structs);

		int playerid = (int)params[1];
		int toplayerid = (int)params[2];
		int animation = (int)params[3];

		if(!IsPlayerConnected(playerid))
			return 0;

		if(!IsPlayerConnected(toplayerid))
			return 0;

		RakNet::BitStream bs;
		Versions::sendSyncData<Structs>(playerid, animation, &bs);

		CSAMPFunctions::Send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, CSAMPFunctions::GetPlayerIDFromIndex(toplayerid), false);

		return 1;
	});
}

// native SendLastSyncPacket(playerid, toplayerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, animation = 0)
static cell AMX_NATIVE_CALL SendLastSyncPacket(AMX *amx, cell *params)
{
	CHECK_PARAMS(4, "SendLastSyncPacket");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		using Structs = decltype(structs);
		using SyncTypes = Global::SyncTypes;

		int playerid = static_cast<int>(params[1]);
		int toplayerid = static_cast<int>(params[2]);
		int type = static_cast<int>(params[3]);
		int animation = static_cast<int>(params[4]);

		if(!IsPlayerConnected(playerid))
			return 0;

		if(!IsPlayerConnected(toplayerid)) 
			return 0;

		RakNet::BitStream bs;

		if(type == SyncTypes::E_LAST_SYNC)
		{
			type = static_cast<int>(Player::lastSyncPacket[playerid]);
		}

		switch(type) 
		{
			case SyncTypes::E_PLAYER_SYNC: // Player Sync
				Versions::sendSyncData<Structs>(playerid, animation, &bs);
				break;

			case SyncTypes::E_AIM_SYNC: // Aim Sync
				Versions::sendAimSyncData<Structs>(playerid, &bs);
				break;

			case SyncTypes::E_VEHICLE_SYNC: // Vehicle Sync
				Versions::sendVehicleSyncData<Structs>(playerid, &bs);
				break;

			case SyncTypes::E_PASSENGER_SYNC: // Passenger Sync
				Versions::sendPassengerSyncData<Structs>(playerid, &bs);
				break;

			case SyncTypes::E_SPECTATING_SYNC: // Spectate Sync
				Versions::sendSpectatingSyncData<Structs>(playerid, &bs);
				break;

			default:
				return 0;
		}

		CSAMPFunctions::Send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, CSAMPFunctions::GetPlayerIDFromIndex(toplayerid), false);

		return 1;
	});
}

// native SetFakeArmour(playerid, health);
static cell AMX_NATIVE_CALL SetFakeArmour(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeArmour");

	int playerid = (int)params[1];
	BYTE armour = (BYTE)params[2];

	if(!IsPlayerConnected(playerid))
		return 0;

	Player::fakeArmour[playerid] = armour;

	return 1;
}

// native SetFakeHealth(playerid, health);
static cell AMX_NATIVE_CALL SetFakeHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeHealth");

	int playerid = (int)params[1];
	BYTE health = (BYTE)params[2];

	if(!IsPlayerConnected(playerid))
		return 0;	

	Player::fakeHealth[playerid] = health;

	return 1;
}

// native SetFakeFacingAngle(playerid, Float:angle)
static cell AMX_NATIVE_CALL SetFakeFacingAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeFacingAngle");

	int playerid = (int)params[1];

	if(!IsPlayerConnected(playerid))
		return 0;	

	if ((int)params[2] == 0x7FFFFFFF)
	{
		Player::fakeQuat[playerid] = NULL;
	}
	else
	{
		glm::vec3 vec = glm::vec3(0.0f, 0.0f, 360.0f - amx_ctof(params[2]));

		Player::fakeQuat[playerid] = new glm::quat(vec * DEG_TO_RAD);
	}

	return 1;
}

// native SetKnifeSync(toggle);
static cell AMX_NATIVE_CALL SetKnifeSync(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SetKnifeSync");

	Global::knifeSync = (BOOL)params[1];

	return 1;
}

// native SetDisableSyncBugs(toggle);
static cell AMX_NATIVE_CALL SetDisableSyncBugs(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SetDisableSyncBugs");

	Global::disableSyncBugs = (BOOL)params[1];

	return 1;
}

// native SetInfiniteAmmoSync(playerid, toggle)
static cell AMX_NATIVE_CALL SetInfiniteAmmoSync(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetInfiniteAmmoSync");

	int playerid = (int)params[1];
	BOOL toggle = (BOOL)params[2];

	if(!IsPlayerConnected(playerid))
		return 0;	

	Player::infiniteAmmo[playerid] = toggle;

	return 1;
}
// native SetKeySyncBlocked(playerid, toggle)
static cell AMX_NATIVE_CALL SetKeySyncBlocked(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetKeySyncBlocked");

	int playerid = (int)params[1];
	BOOL toggle = (BOOL)params[2];

	if(!IsPlayerConnected(playerid))
		return 0;	

	Player::blockKeySync[playerid] = toggle;

	return 1;
}

// native ClearAnimationsForPlayer(playerid, forplayerid)
static cell AMX_NATIVE_CALL ClearAnimationsForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "ClearAnimationsForPlayer");

	int playerid = (int)params[1];
	int forplayerid = (int)params[2];

	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsPlayerConnected(forplayerid)) 
		return 0;

	RakNet::BitStream bs;
	bs.Write((WORD)playerid);

	CSAMPFunctions::RPC(&RPC_ClearAnimations, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, CSAMPFunctions::GetPlayerIDFromIndex(forplayerid), false, false);

	return 1;
}

// native SendDeath(playerid);
static cell AMX_NATIVE_CALL SendDeath(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SendDeath");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		int playerid = (int)params[1];

		if(!IsPlayerConnected(playerid))
			return 0;		

		auto *pPlayer = netGame->pPlayerPool->pPlayer[playerid];

		pPlayer->byteState = PLAYER_STATE_WASTED;

		RakNet::BitStream bs;
		bs.Write((WORD)playerid);

		CSAMPFunctions::RPC(&RPC_DeathBroadcast, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, CSAMPFunctions::GetPlayerIDFromIndex(playerid), true, false);

		return 1;
	});
}

// native FreezeSyncData(playerid, bool:toggle)
static cell AMX_NATIVE_CALL FreezeSyncData(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "FreezeSyncData");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		using Structs = decltype(structs);
		using SyncTypes = Global::SyncTypes;

		int playerid = (int)params[1];
		BOOL toggle = (BOOL)params[2];

		if(!IsPlayerConnected(playerid))
			return 0;		

		auto *d = &Player::getLastSyncData<Structs>(playerid);
		d->vecVelocity = CVector();
		d->byteSpecialAction = 0;
		d->wKeys = 0;
		d->wUDAnalog = 0;
		d->wLRAnalog = 0;

		Player::SetSyncFrozenState(playerid, SyncTypes::E_PLAYER_SYNC, toggle);

		return 1;
	});
}

// native FreezeSyncPacket(playerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, bool:toggle)
static cell AMX_NATIVE_CALL FreezeSyncPacket(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "FreezeSyncPacket");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		using Structs = decltype(structs);
		using SyncTypes = Global::SyncTypes;

		int playerid = static_cast<int>(params[1]);
		int type = static_cast<int>(params[2]);
		BOOL toggle = static_cast<BOOL>(params[3]);

		if(!IsPlayerConnected(playerid))
			return 0;

		if(type == SyncTypes::E_LAST_SYNC)
		{
			type = static_cast<int>(Player::lastSyncPacket[playerid]);			
		}

		switch(type)
		{
			case SyncTypes::E_PLAYER_SYNC: // Player Sync
			{
				auto *d = &Player::getLastSyncData<Structs>(playerid);
				d->vecVelocity = CVector();
				d->byteSpecialAction = 0;
				d->wKeys = 0;
				d->wUDAnalog = 0;
				d->wLRAnalog = 0;

				Player::SetSyncFrozenState(playerid, SyncTypes::E_PLAYER_SYNC, toggle);
				break;
			}

			case SyncTypes::E_AIM_SYNC: // Aim Sync
			{
				Player::syncAimDataFrozen[playerid] = toggle;
				break;				
			}

			case SyncTypes::E_VEHICLE_SYNC: // Vehicle Sync
			{
				auto *d = &Player::getLastVehicleSyncData<Structs>(playerid);
				d->vecVelocity = CVector();
				d->wKeys = 0;
				d->wUDAnalog = 0;
				d->wLRAnalog = 0;

				Player::SetSyncFrozenState(playerid, SyncTypes::E_VEHICLE_SYNC, toggle);				
				break;
			}

			case SyncTypes::E_PASSENGER_SYNC: // Passenger Sync
			{
				auto *d = &Player::getLastPassengerSyncData<Structs>(playerid);
				d->wKeys = 0;
				d->wUDAnalog = 0;
				d->wLRAnalog = 0;

				Player::SetSyncFrozenState(playerid, SyncTypes::E_PASSENGER_SYNC, toggle);
				break;				
			}

			case SyncTypes::E_SPECTATING_SYNC: // Spectate Sync
			{
				auto *d = &Player::getLastSpectatingSyncData<Structs>(playerid);
				d->wKeysOnSpectating = 0;
				d->wUpDownKeysOnSpectating = 0;
				d->wLeftRightKeysOnSpectating = 0;

				Player::SetSyncFrozenState(playerid, SyncTypes::E_SPECTATING_SYNC, toggle);
				break;			
			}

			case SyncTypes::E_ALL_SYNC: // All Syncs
			{
				Player::SetSyncFrozenState(playerid, SyncTypes::E_ALL_SYNC, toggle);
				break;
			}

			default:
				return 0;
		}

		return 1;
	});
}

// native TextDrawSetPosition(Text:text, Float:fX, Float:fY)
static cell AMX_NATIVE_CALL TextDrawSetPosition(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "TextDrawSetPosition");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		int textdrawid = (int)params[1];
		if (textdrawid < 0 || textdrawid >= MAX_TEXT_DRAWS)
			return 0;

		if (!netGame || !netGame->pTextDrawPool || !netGame->pTextDrawPool->bSlotState[textdrawid])
			return 0;

		auto *pTD = netGame->pTextDrawPool->TextDraw[textdrawid];

		pTD->vecPos.fX = amx_ctof(params[2]);
		pTD->vecPos.fY = amx_ctof(params[3]);

		return 1;
	});
}

// native PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:fX, Float:fY)
static cell AMX_NATIVE_CALL PlayerTextDrawSetPosition(AMX *amx, cell *params)
{
	CHECK_PARAMS(4, "PlayerTextDrawSetPosition");

	return Versions::getNetGame([params](auto netGame, auto structs) {
		int playerid = (int)params[1];
		int textdrawid = (int)params[2];

		if(!IsPlayerConnected(playerid))
			return 0;

		if (textdrawid < 0 || textdrawid >= MAX_PLAYER_TEXT_DRAWS)
			return 0;

		if (!netGame->pPlayerPool->pPlayer[playerid])
			return 0;
		if (!netGame->pPlayerPool->pPlayer[playerid]->pTextdraw->bSlotState[textdrawid])
			return 0;

		auto *pTD = netGame->pPlayerPool->pPlayer[playerid]->pTextdraw->TextDraw[textdrawid];

		pTD->vecPos.fX = amx_ctof(params[3]);
		pTD->vecPos.fY = amx_ctof(params[4]);

		return 1;
	});
}

// native TextDrawSetStringForPlayer(Text:text, playerid, const string[])
static cell AMX_NATIVE_CALL TextDrawSetStrForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "TextDrawSetStrForPlayer");

	int textdrawid = (int)params[1];
	if (textdrawid < 0 || textdrawid >= MAX_TEXT_DRAWS)
		return 0;

	int playerid = (int)params[2];
	if(!IsPlayerConnected(playerid))
		return 0;
			
	char *text;
	amx_StrParam(amx, params[3], text);
	unsigned short len = (unsigned short)strlen(text);
	
	RakNet::BitStream bs;
	bs.Write((WORD)textdrawid);
	bs.Write((unsigned short)len);
	bs.Write(text, len + 1);

	CSAMPFunctions::RPC(&RPC_ScrEditTextDraw, &bs, HIGH_PRIORITY, RELIABLE, 0, CSAMPFunctions::GetPlayerIDFromIndex(playerid), false, false);

	return 1;
}

static AMX_NATIVE_INFO native_list[] = {
	{"SpawnPlayerForWorld", SpawnPlayerForWorld},
	{"SetFakeHealth", SetFakeHealth},
	{"SetFakeArmour", SetFakeArmour},
	{"SetFakeFacingAngle", SetFakeFacingAngle},
	{"FreezeSyncData", FreezeSyncData},
	{"FreezeSyncPacket", FreezeSyncPacket},
	{"SetKnifeSync", SetKnifeSync},
	{"SendDeath", SendDeath},
	{"SetLastAnimationData", SetLastAnimationData},
	{"SendLastSyncData", SendLastSyncData},
	{"SendLastSyncPacket", SendLastSyncPacket},
	{"SetDisableSyncBugs", SetDisableSyncBugs},
	{"ClearAnimationsForPlayer", ClearAnimationsForPlayer},
	{"SetKeySyncBlocked", SetKeySyncBlocked},
	{"SetInfiniteAmmoSync", SetInfiniteAmmoSync},
	{"TextDrawSetPosition", TextDrawSetPosition},
	{"PlayerTextDrawSetPosition", PlayerTextDrawSetPosition},
	{"TextDrawSetStrForPlayer", TextDrawSetStrForPlayer},
	{0, 0}};

int InitScripting(AMX *amx)
{
	return amx_Register(amx, native_list, -1);
}
