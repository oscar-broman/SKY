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

#include "scripting.hpp"

#include "main.hpp"
#include "player.hpp"
#include "global.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <limits>

#ifdef _WIN32
#include <windows.h>
// Yes - BOTH string versions...
#include <strsafe.h>
#else
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <algorithm>
#endif

#include <stdio.h>
#include <string.h>

//----------------------------------------------------
const float MATH_PI = 3.14159265359f;
const float RAD_TO_DEG = 180.0f / MATH_PI;
const float DEG_TO_RAD = 1.0f / RAD_TO_DEG;

// native SpawnPlayerForWorld(playerid);
static cell AMX_NATIVE_CALL SpawnPlayerForWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SpawnPlayerForWorld");

	auto *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	FlatPtrHashSet<IPlayer> streamedPlayers = player->streamedForPlayers();

	for (IPlayer *other : streamedPlayers)
	{
		player->streamOutForPlayer(*other);
	}

	return 1;
}

// native SetLastAnimationData(playerid, data)
static cell AMX_NATIVE_CALL SetLastAnimationData(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetLastAnimationData");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	// TODO: wait for open.mp to implement a way to to modify animation data without sending rpc

	return 1;
}

// native SendLastSyncData(playerid, toplayerid, animation = 0)
static cell AMX_NATIVE_CALL SendLastSyncData(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "SendLastSyncData");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	IPlayer *toPlayer = SkyComponent::getCore()->getPlayers().get(params[2]);
	if (toPlayer == nullptr)
	{
		return 0;
	}

	NetCode::Packet::PlayerFootSync &pkt = Player::lastSyncData[player->getID()];
	if (!params[2])
	{
		pkt.AnimationID = 0;
	}

	PacketHelper::send<NetCode::Packet::PlayerFootSync>(
		pkt,
		*toPlayer);

	return 1;
}

// native SendLastSyncPacket(playerid, toplayerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, animation = 0)
static cell AMX_NATIVE_CALL SendLastSyncPacket(AMX *amx, cell *params)
{
	CHECK_PARAMS(4, "SendLastSyncPacket");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	IPlayer *toPlayer = SkyComponent::getCore()->getPlayers().get(params[2]);
	if (toPlayer == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();

	switch (params[2])
	{
	case SyncTypes::E_PLAYER_SYNC:
	{
		NetCode::Packet::PlayerFootSync pkt = Player::lastSyncData[playerId];
		if (!params[3])
		{
			pkt.AnimationID = 0;
		}

		PacketHelper::send<NetCode::Packet::PlayerFootSync>(
			pkt,
			*toPlayer);
	}
	case SyncTypes::E_AIM_SYNC:
	{
		PacketHelper::send<NetCode::Packet::PlayerAimSync>(
			Player::lastPlayerAimSyncData[playerId],
			*toPlayer);
	}
	case SyncTypes::E_VEHICLE_SYNC:
	{
		PacketHelper::send<NetCode::Packet::PlayerVehicleSync>(
			Player::lastVehicleSyncData[playerId],
			*toPlayer);
	}
	case SyncTypes::E_PASSENGER_SYNC:
	{
		PacketHelper::send<NetCode::Packet::PlayerPassengerSync>(
			Player::lastPlayerPassengerSyncData[playerId],
			*toPlayer);
	}
	case SyncTypes::E_SPECTATING_SYNC:
	{
		PacketHelper::send<NetCode::Packet::PlayerSpectatorSync>(
			Player::lastPlayerSpectateSyncData[playerId],
			*toPlayer);
	}
	default:
	{
		return 0;
	}
	}

	return 1;
}

// native SetFakeArmour(playerid, health);
static cell AMX_NATIVE_CALL SetFakeArmour(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeArmour");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();
	Player::fakeArmour[playerId] = (float)params[2];

	return 1;
}

// native SetFakeHealth(playerid, health);
static cell AMX_NATIVE_CALL SetFakeHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeHealth");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();
	Player::fakeHealth[playerId] = (float)params[2];

	return 1;
}

// native SetFakeFacingAngle(playerid, Float:angle)
static cell AMX_NATIVE_CALL SetFakeFacingAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetFakeFacingAngle");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();

	if (Player::fakeQuat[playerId])
	{
		delete Player::fakeQuat[playerId];
	}

	if ((int)params[2] == 0x7FFFFFFF)
	{
		Player::fakeQuat[playerId] = nullptr;
	}
	else
	{
		float angle = 360.0f - amx_ctof(params[2]);
		*Player::fakeQuat[playerId] = Vector4(0.0f, 0.0f, 0.0f, angle * DEG_TO_RAD);
	}

	return 1;
}

// native SetKnifeSync(toggle);
static cell AMX_NATIVE_CALL SetKnifeSync(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SetKnifeSync");

	knifeSync = (bool)params[1];

	return 1;
}

// native SetDisableSyncBugs(toggle);
static cell AMX_NATIVE_CALL SetDisableSyncBugs(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SetDisableSyncBugs");

	disableSyncBugs = (bool)params[1];

	return 1;
}

// native SetInfiniteAmmoSync(playerid, toggle)
static cell AMX_NATIVE_CALL SetInfiniteAmmoSync(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetInfiniteAmmoSync");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();
	Player::infiniteAmmo[playerId] = (bool)params[2];

	return 1;
}

// native SetKeySyncBlocked(playerid, toggle)
static cell AMX_NATIVE_CALL SetKeySyncBlocked(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "SetKeySyncBlocked");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();
	Player::blockKeySync[playerId] = (bool)params[2];

	return 1;
}

// native ClearAnimationsForPlayer(playerid, forplayerid)
static cell AMX_NATIVE_CALL ClearAnimationsForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "ClearAnimationsForPlayer");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	IPlayer *forPlayer = SkyComponent::getCore()->getPlayers().get(params[2]);
	if (player == nullptr)
	{
		return 0;
	}

	NetCode::RPC::ClearPlayerAnimations cpa;
	cpa.PlayerID = player->getID();

	PacketHelper::send<NetCode::RPC::ClearPlayerAnimations>(
		cpa,
		*forPlayer);

	return 1;
}

// native SendDeath(playerid);
static cell AMX_NATIVE_CALL SendDeath(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "SendDeath");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	// TODO: waiting for open.mp to implement a way to set a players state
	// player->setState(PLAYER_STATE_WASTED);

	NetCode::RPC::PlayerDeath pd;
	pd.PlayerID = player->getID();

	PacketHelper::send<NetCode::RPC::PlayerDeath>(
		pd,
		*player);

	return 1;
}

// native FreezeSyncData(playerid, bool:toggle)
static cell AMX_NATIVE_CALL FreezeSyncData(AMX *amx, cell *params)
{
	CHECK_PARAMS(2, "FreezeSyncData");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();

	NetCode::Packet::PlayerFootSync data = Player::lastSyncData[playerId];
	data.Velocity = Vector3();
	data.SpecialAction = SpecialAction_None;
	data.Keys = 0;
	data.LeftRight = 0;
	data.UpDown = 0;

	bool toggle = (bool)params[2];
	Player::SetSyncFrozenState(playerId, SyncTypes::E_PLAYER_SYNC, toggle);

	return 1;
}

// native FreezeSyncPacket(playerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, bool:toggle)
static cell AMX_NATIVE_CALL FreezeSyncPacket(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "FreezeSyncPacket");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	int playerId = player->getID();
	bool toggle = (bool)params[3];

	switch (params[2])
	{
	case SyncTypes::E_PLAYER_SYNC:
	{
		NetCode::Packet::PlayerFootSync data = Player::lastSyncData[playerId];
		data.Velocity = Vector3();
		data.SpecialAction = SpecialAction_None;
		data.Keys = 0;
		data.LeftRight = 0;
		data.UpDown = 0;

		Player::SetSyncFrozenState(playerId, SyncTypes::E_PLAYER_SYNC, toggle);
	}
	case SyncTypes::E_AIM_SYNC:
	{
		Player::SetSyncFrozenState(playerId, SyncTypes::E_AIM_SYNC, toggle);
	}
	case SyncTypes::E_VEHICLE_SYNC:
	{
		NetCode::Packet::PlayerVehicleSync data = Player::lastVehicleSyncData[playerId];
		data.Velocity = Vector3();
		data.Keys = 0;
		data.LeftRight = 0;
		data.UpDown = 0;

		Player::SetSyncFrozenState(playerId, SyncTypes::E_VEHICLE_SYNC, toggle);
	}
	case SyncTypes::E_PASSENGER_SYNC:
	{
		NetCode::Packet::PlayerPassengerSync data = Player::lastPlayerPassengerSyncData[playerId];
		data.Keys = 0;
		data.LeftRight = 0;
		data.UpDown = 0;

		Player::SetSyncFrozenState(playerId, SyncTypes::E_PASSENGER_SYNC, toggle);
	}
	case SyncTypes::E_SPECTATING_SYNC:
	{
		NetCode::Packet::PlayerSpectatorSync data = Player::lastPlayerSpectateSyncData[playerId];
		data.Keys = 0;
		data.LeftRight = 0;
		data.UpDown = 0;

		Player::SetSyncFrozenState(playerId, SyncTypes::E_SPECTATING_SYNC, toggle);
	}
	case SyncTypes::E_ALL_SYNC:
	{
		Player::SetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC, toggle);
	}
	default:
	{
		return 0;
	}
	}

	return 1;
}

// native TextDrawSetPosition(Text:text, Float:fX, Float:fY)
static cell AMX_NATIVE_CALL TextDrawSetPosition(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "TextDrawSetPosition");

	ITextDraw *td = SkyComponent::getTextDraws()->get(params[1]);
	if (td == nullptr)
	{
		return 0;
	}

	float fX = amx_ctof(params[2]);
	float fY = amx_ctof(params[3]);

	td->setPosition(Vector2(fX, fY));

	return 1;
}

// native PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:fX, Float:fY)
static cell AMX_NATIVE_CALL PlayerTextDrawSetPosition(AMX *amx, cell *params)
{
	CHECK_PARAMS(4, "PlayerTextDrawSetPosition");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[1]);
	if (player == nullptr)
	{
		return 0;
	}

	float fX = amx_ctof(params[3]);
	float fY = amx_ctof(params[4]);

	auto td_data = queryExtension<IPlayerTextDrawData>(*player);
	if (td_data)
	{
		auto ptd = td_data->get(params[2]);
		ptd->setPosition(Vector2(fX, fY));
	}

	return 1;
}

// native TextDrawSetStringForPlayer(Text:text, playerid, const string[])
static cell AMX_NATIVE_CALL TextDrawSetStrForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3, "TextDrawSetStrForPlayer");

	IPlayer *player = SkyComponent::getCore()->getPlayers().get(params[2]);
	if (player == nullptr)
	{
		return 0;
	}

	ITextDraw *td = SkyComponent::getTextDraws()->get(params[1]);
	if (td == nullptr)
	{
		return 0;
	}

	char *text;
	amx_StrParam(amx, params[3], text);

	NetCode::RPC::PlayerTextDrawSetString tdp;
	tdp.TextDrawID = td->getID();
	tdp.Text = text;

	PacketHelper::send<NetCode::RPC::PlayerTextDrawSetString>(
		tdp,
		*player);

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
