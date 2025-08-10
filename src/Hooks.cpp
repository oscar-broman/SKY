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
#include "Functions.h"
#include "Player.h"
#include "RPCs.h"
#include "Scripting.h"
#include "Utils.h"
#include "Versions.h"
#include "main.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef _WIN32
#include <psapi.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <cstdarg>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#endif

#ifndef PAGESIZE
#define PAGESIZE (4096)
#endif

using std::isfinite;
extern void *pAMXFunctions;

namespace AnimationIDs
{
	// PED_RUN animations
	constexpr WORD PED_RUN_START = 1222;
	constexpr WORD PED_RUN_END = 1236;
	constexpr WORD PED_SWAT_RUN = 1249;

	// PED_WOMAN animations
	constexpr WORD PED_WOMAN_START = 1275;
	constexpr WORD PED_WOMAN_END = 1287;

	// Other movement animations
	constexpr WORD FAT_FATRUN_ARMED = 459;
	constexpr WORD MUSCULAR_MUSCLERUN1 = 908;
	constexpr WORD MUSCULAR_MUSCLERUN2 = 909;
	constexpr WORD PED_WEAPON_CROUCH = 1274;
	constexpr WORD PED_WALK_PLAYER = 1266;
	constexpr WORD PED_SHOT_PARTIAL1 = 1241;
	constexpr WORD PED_SHOT_PARTIAL2 = 1242;

	// Baseball bat animations
	constexpr WORD BASEBALL_BAT_START = 17;
	constexpr WORD BASEBALL_BAT_END = 27;

	// Knife animations
	constexpr WORD KNIFE_START = 745;
	constexpr WORD KNIFE_END = 760;

	// Sword animations
	constexpr WORD SWORD_START = 1545;
	constexpr WORD SWORD_END = 1554;

	// Fight animations
	constexpr WORD FIGHT_START1 = 471;
	constexpr WORD FIGHT_END1 = 507;
	constexpr WORD FIGHT_START2 = 1135;
	constexpr WORD FIGHT_END2 = 1151;

	// Spray can animations
	constexpr WORD SPRAYCAN_START = 1160;
	constexpr WORD SPRAYCAN_END = 1167;

	// Grenade animations
	constexpr WORD GRENADE_START = 644;
	constexpr WORD GRENADE_END = 646;
}

static bool IsProblematicAnimation(WORD animIndex)
{
	using namespace AnimationIDs;

	return (animIndex >= PED_RUN_START && animIndex <= PED_RUN_END) ||
		   (animIndex == PED_SWAT_RUN) ||
		   (animIndex >= PED_WOMAN_START && animIndex <= PED_WOMAN_END) ||
		   (animIndex == FAT_FATRUN_ARMED) ||
		   (animIndex == MUSCULAR_MUSCLERUN1 || animIndex == MUSCULAR_MUSCLERUN2) ||
		   (animIndex == PED_WEAPON_CROUCH) ||
		   (animIndex == PED_WALK_PLAYER) ||
		   (animIndex == PED_SHOT_PARTIAL1 || animIndex == PED_SHOT_PARTIAL2) ||
		   (animIndex >= BASEBALL_BAT_START && animIndex <= BASEBALL_BAT_END) ||
		   (animIndex >= KNIFE_START && animIndex <= KNIFE_END) ||
		   (animIndex >= SWORD_START && animIndex <= SWORD_END) ||
		   (animIndex >= FIGHT_START1 && animIndex <= FIGHT_END1) ||
		   (animIndex >= FIGHT_START2 && animIndex <= FIGHT_END2);
}

template <typename SyncData>
static void ApplyGhostShootingPrevention(SyncData *data)
{
	if (!Global::disableSyncBugs)
	{
		return;
	}

	if ((data->byteWeapon >= WEAPON_COLT45 && data->byteWeapon <= WEAPON_SNIPER) ||
		data->byteWeapon == WEAPON_MINIGUN)
	{

		if (IsProblematicAnimation(data->wAnimIndex))
		{
			// Only remove action key if holding aim
			if (data->wKeys & 128)
			{
				data->wKeys &= ~1; // Remove action key
			}
			data->wKeys &= ~4;	 // Remove fire key
			data->wKeys &= ~128; // Remove aim key
		}
	}
	else if (data->byteWeapon == WEAPON_SPRAYCAN ||
			 data->byteWeapon == WEAPON_FIREEXTINGUISHER ||
			 data->byteWeapon == WEAPON_FLAMETHROWER)
	{

		if (data->wAnimIndex < AnimationIDs::SPRAYCAN_START ||
			data->wAnimIndex > AnimationIDs::SPRAYCAN_END)
		{

			if (data->wKeys & 128)
			{
				data->wKeys &= ~1; // Remove action key
			}
			data->wKeys &= ~4;	 // Remove fire key
			data->wKeys &= ~128; // Remove aim key
		}
	}
	else if (data->byteWeapon == WEAPON_GRENADE)
	{
		if (data->wAnimIndex < AnimationIDs::GRENADE_START ||
			data->wAnimIndex > AnimationIDs::GRENADE_END)
		{
			data->wKeys &= ~1; // Remove action key
		}
	}
}

template <typename Structs, typename SyncType>
static void HandlePlayerSyncFreezing(WORD playerId, typename Structs::CSyncData *data, SyncType syncType)
{
	using SyncTypes = Global::SyncTypes;

	auto lastSyncData = &Player::getLastSyncData<Structs>(playerId);

	if (Player::GetSyncFrozenState(playerId, syncType) ||
		Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
	{
		std::memcpy(data, lastSyncData, sizeof(typename Structs::CSyncData));
	}
	else
	{
		std::memcpy(lastSyncData, data, sizeof(typename Structs::CSyncData));
	}
}

template <typename Structs, typename SyncType>
static void HandleAimSyncFreezing(WORD playerId, typename Structs::CAimSyncData *data, SyncType syncType)
{
	using SyncTypes = Global::SyncTypes;

	auto lastAimSyncData = &Player::getLastAimSyncData<Structs>(playerId);

	if (Player::GetSyncFrozenState(playerId, syncType) ||
		Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
	{
		std::memcpy(data, lastAimSyncData, sizeof(typename Structs::CAimSyncData));
	}
	else
	{
		std::memcpy(lastAimSyncData, data, sizeof(typename Structs::CAimSyncData));
	}
}

template <typename Structs, typename SyncType>
static void HandleVehicleSyncFreezing(WORD playerId, typename Structs::CVehicleSyncData *data, SyncType syncType)
{
	using SyncTypes = Global::SyncTypes;

	auto lastVehicleSyncData = &Player::getLastVehicleSyncData<Structs>(playerId);

	if (Player::GetSyncFrozenState(playerId, syncType) ||
		Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
	{
		std::memcpy(data, lastVehicleSyncData, sizeof(typename Structs::CVehicleSyncData));
	}
	else
	{
		std::memcpy(lastVehicleSyncData, data, sizeof(typename Structs::CVehicleSyncData));
	}
}

template <typename Structs, typename SyncType>
static void HandlePassengerSyncFreezing(WORD playerId, typename Structs::CPassengerSyncData *data, SyncType syncType)
{
	using SyncTypes = Global::SyncTypes;

	auto lastPassengerSyncData = &Player::getLastPassengerSyncData<Structs>(playerId);

	if (Player::GetSyncFrozenState(playerId, syncType) ||
		Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
	{
		std::memcpy(data, lastPassengerSyncData, sizeof(typename Structs::CPassengerSyncData));
	}
	else
	{
		std::memcpy(lastPassengerSyncData, data, sizeof(typename Structs::CPassengerSyncData));
	}
}

template <typename Structs, typename SyncType>
static void HandleSpectatorSyncFreezing(WORD playerId, typename Structs::CSpectatingSyncData *data, SyncType syncType)
{
	using SyncTypes = Global::SyncTypes;

	auto lastSpectatingSyncData = &Player::getLastSpectatingSyncData<Structs>(playerId);

	if (Player::GetSyncFrozenState(playerId, syncType) ||
		Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
	{
		std::memcpy(data, lastSpectatingSyncData, sizeof(typename Structs::CSpectatingSyncData));
	}
	else
	{
		std::memcpy(lastSpectatingSyncData, data, sizeof(typename Structs::CSpectatingSyncData));
	}
}

template <typename SyncData>
static void ApplyFakePlayerStats(WORD playerId, SyncData *data)
{
	if (Player::fakeHealth[playerId] != 255)
	{
		data->byteHealth = Player::fakeHealth[playerId];
	}

	if (Player::fakeArmour[playerId] != 255)
	{
		data->byteArmour = Player::fakeArmour[playerId];
	}
}

template <typename SyncData>
static void ApplyFakeVehicleStats(WORD playerId, SyncData *data)
{
	if (Player::fakeHealth[playerId] != 255)
	{
		data->bytePlayerHealth = Player::fakeHealth[playerId];
	}

	if (Player::fakeArmour[playerId] != 255)
	{
		data->bytePlayerArmour = Player::fakeArmour[playerId];
	}
}

bool Unlock(void *address, size_t len)
{
#ifdef _WIN32
	DWORD oldProtection;
	return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldProtection);
#else
	size_t pageSize = getpagesize();
	size_t alignedAddr = ((reinterpret_cast<uint32_t>(address) / pageSize) * pageSize);
	return !mprotect(reinterpret_cast<void *>(alignedAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for (; *mask; ++mask, ++data, ++pattern)
	{
		if (*mask == 'x' && *data != *pattern)
		{
			return false;
		}
	}
	return (*mask) == 0;
}

DWORD FindPattern(const char *pattern, const char *mask)
{
	DWORD address;
	DWORD size;

#ifdef _WIN32
	MODULEINFO info = {0};
	address = (DWORD)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (DWORD)info.SizeOfImage;
#else
	address = 0x804b480; // around the elf base
	size = 0x8128B80 - address;
#endif

	for (DWORD i = 0; i < size; ++i)
	{
		if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
		{
			return (DWORD)(address + i);
		}
	}
	return 0;
}

static bool IsPlayerUpdatePacket(unsigned char packetId)
{
	return (packetId == ID_PLAYER_SYNC ||
			packetId == ID_VEHICLE_SYNC ||
			packetId == ID_PASSENGER_SYNC ||
			packetId == ID_SPECTATOR_SYNC ||
			packetId == ID_AIM_SYNC ||
			packetId == ID_TRAILER_SYNC);
}

BYTE GetPacketID(Packet *packet)
{
	if (packet == nullptr)
	{
		return 255;
	}

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		if (packet->length <= sizeof(unsigned char) + sizeof(unsigned long))
		{
			return 255;
		}
		return (unsigned char)packet->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}

	return (unsigned char)packet->data[0];
}

template <typename Structs>
static bool ProcessPlayerSync(Packet *packet, WORD playerId)
{
	using SyncTypes = Global::SyncTypes;

	if (packet->length != (sizeof(typename Structs::CSyncData) + 1))
	{
		return false;
	}

	auto *data = reinterpret_cast<typename Structs::CSyncData *>(&packet->data[1]);

	// Validate positions to prevent infinite loops from NaN values
	if (!ValidatePosition(data->vecPosition) ||
		!ValidatePosition(data->vecSurfing) ||
		!ValidatePosition(data->vecVelocity))
	{
		return false;
	}

	if (!ValidateWeaponID(data->byteWeapon))
	{
		data->byteWeapon = 0; // Set to unarmed
	}

	// Fix detonator crasher - removes AIM_KEY for detonator
	if (data->byteWeapon == 40)
	{
		data->wKeys &= ~128;
	}

	ApplyGhostShootingPrevention(data);

	HandlePlayerSyncFreezing<Structs>(playerId, data, SyncTypes::E_PLAYER_SYNC);

	if (Player::blockKeySync[playerId])
	{
		data->wKeys = 0;
	}

	ApplyFakePlayerStats(playerId, data);

	// Apply fake quaternion if set
	if (Player::fakeQuat[playerId] != NULL)
	{
		data->fQuaternion[0] = Player::fakeQuat[playerId]->w; // angle
		data->fQuaternion[1] = Player::fakeQuat[playerId]->x; // x
		data->fQuaternion[2] = Player::fakeQuat[playerId]->y; // y
		data->fQuaternion[3] = Player::fakeQuat[playerId]->z; // z
	}

	// Handle special weapon key restrictions
	if (data->byteWeapon == 44 || data->byteWeapon == 45)
	{
		data->wKeys &= ~4; // Remove fire key for night vision/thermal goggles
	}
	else if (data->byteWeapon == 4 && !Global::knifeSync)
	{
		data->wKeys &= ~128; // Remove aim key for knife if knife sync disabled
	}

	Player::lastWeapon[playerId] = data->byteWeapon;
	Player::lastSyncPacket[playerId] = SyncTypes::E_PLAYER_SYNC;

	return true;
}

template <typename Structs>
static bool ProcessAimSync(Packet *packet, WORD playerId)
{
	using SyncTypes = Global::SyncTypes;

	if (packet->length != (sizeof(typename Structs::CAimSyncData) + 1))
	{
		return false;
	}

	auto *data = reinterpret_cast<typename Structs::CAimSyncData *>(&packet->data[1]);

	if (!ValidatePosition(data->vecFront) ||
		!ValidatePosition(data->vecPosition))
	{
		return false;
	}

	HandleAimSyncFreezing<Structs>(playerId, data, SyncTypes::E_AIM_SYNC);

	// Fix first-person up/down aim sync for sniper rifles
	if (Player::lastWeapon[playerId] == 34 ||
		Player::lastWeapon[playerId] == 35 ||
		Player::lastWeapon[playerId] == 36 ||
		Player::lastWeapon[playerId] == 43)
	{

		data->fZAim = -data->vecFront.fZ;
		data->fZAim = std::clamp(data->fZAim, -1.0f, 1.0f);
	}

	if (Player::infiniteAmmo[playerId])
	{
		data->byteCameraZoom = 2;
	}

	Player::lastSyncPacket[playerId] = SyncTypes::E_AIM_SYNC;
	return true;
}

template <typename Structs>
static bool ProcessVehicleSync(Packet *packet, WORD playerId)
{
	using SyncTypes = Global::SyncTypes;

	if (packet->length != (sizeof(typename Structs::CVehicleSyncData) + 1))
	{
		return false;
	}

	auto *data = reinterpret_cast<typename Structs::CVehicleSyncData *>(&packet->data[1]);

	if (!ValidateVehicleID(data->wVehicleId))
	{
		return false;
	}

	if (!ValidatePosition(data->vecPosition) ||
		!ValidatePosition(data->vecVelocity))
	{
		return false;
	}

	HandleVehicleSyncFreezing<Structs>(playerId, data, SyncTypes::E_VEHICLE_SYNC);

	if (!ValidateWeaponID(data->bytePlayerWeapon))
	{
		data->bytePlayerWeapon = 0; // Set to unarmed
	}

	ApplyFakeVehicleStats(playerId, data);

	Player::lastSyncPacket[playerId] = SyncTypes::E_VEHICLE_SYNC;
	return true;
}

template <typename Structs>
static bool ProcessPassengerSync(Packet *packet, WORD playerId)
{
	using SyncTypes = Global::SyncTypes;

	if (packet->length != (sizeof(typename Structs::CPassengerSyncData) + 1))
	{
		return false;
	}

	auto *data = reinterpret_cast<typename Structs::CPassengerSyncData *>(&packet->data[1]);

	if (!ValidateVehicleID(data->wVehicleId))
	{
		return false;
	}

	if (!ValidatePosition(data->vecPosition))
	{
		return false;
	}

	HandlePassengerSyncFreezing<Structs>(playerId, data, SyncTypes::E_PASSENGER_SYNC);

	if (!ValidateWeaponID(data->bytePlayerWeapon))
	{
		data->bytePlayerWeapon = 0; // Set to unarmed
	}

	ApplyFakeVehicleStats(playerId, data);

	Player::lastSyncPacket[playerId] = SyncTypes::E_PASSENGER_SYNC;
	return true;
}

template <typename Structs>
static bool ProcessSpectatorSync(Packet *packet, WORD playerId)
{
	using SyncTypes = Global::SyncTypes;

	if (packet->length != (sizeof(typename Structs::CSpectatingSyncData) + 1))
	{
		return false;
	}

	auto *data = reinterpret_cast<typename Structs::CSpectatingSyncData *>(&packet->data[1]);

	if (!ValidatePosition(data->vecPosition))
	{
		return false;
	}

	HandleSpectatorSyncFreezing<Structs>(playerId, data, SyncTypes::E_SPECTATING_SYNC);

	Player::lastSyncPacket[playerId] = SyncTypes::E_SPECTATING_SYNC;
	return true;
}

Packet *THISCALL CHookRakServer::Receive(void *ppRakServer)
{
	return Versions::getNetGame([ppRakServer](auto netGame, auto structs) -> Packet *
								{
        using Structs = decltype(structs);

        Packet *packet = CSAMPFunctions::Receive(ppRakServer);

        BYTE packetId = GetPacketID(packet);
        if (packetId == 0xFF) {
            return packet;
        }

        WORD playerId = packet->playerIndex;
        if (!ValidatePlayerID(playerId)) {
            return nullptr;
        }

        if (!Player::CheckPacketRateLimit(playerId)) {
            return nullptr; // Rate limit exceeded, drop packet silently
        }

        Player::lastUpdateTick[playerId] = GetTickCount();

        switch (packetId) {
            case ID_PLAYER_SYNC:
                if (!ProcessPlayerSync<Structs>(packet, playerId)) {
                    return nullptr;
                }
                break;

            case ID_AIM_SYNC:
                if (!ProcessAimSync<Structs>(packet, playerId)) {
                    return nullptr;
                }
                break;

            case ID_VEHICLE_SYNC:
                if (!ProcessVehicleSync<Structs>(packet, playerId)) {
                    return nullptr;
                }
                break;

            case ID_PASSENGER_SYNC:
                if (!ProcessPassengerSync<Structs>(packet, playerId)) {
                    return nullptr;
                }
                break;

            case ID_SPECTATOR_SYNC:
                if (!ProcessSpectatorSync<Structs>(packet, playerId)) {
                    return nullptr;
                }
                break;

            default:
                // For other packet types, just pass through
                break;
        }

        return packet; });
}

//----------------------------------------------------

/**
 * @brief Initialize hook system and player data
 */
void InstallPreHooks()
{
	// Initialize fake stat arrays
	std::memset(&Player::fakeHealth, 255, sizeof(Player::fakeHealth));
	std::memset(&Player::fakeArmour, 255, sizeof(Player::fakeArmour));
	std::memset(&Player::fakeQuat[0], NULL, sizeof(Player::fakeQuat));

	// Initialize rate limits for all players
	for (int playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		Player::InitializeRateLimits(playerId);
	}
}
