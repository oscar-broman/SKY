#pragma once

#include "Player.h"
#include "Utils.h"
#include "Global.h"
#include "Structs.h"
#include "net.h"
#include "Animation.h"
#include "SyncFreezing.h"
#include <algorithm>

namespace SyncProcessing
{
    template <typename SyncData>
    void ApplyGhostShootingPrevention(SyncData *data)
    {
        if (!Global::disableSyncBugs) {
            return;
        }

        if ((data->byteWeapon >= WEAPON_COLT45 && data->byteWeapon <= WEAPON_SNIPER) ||
            data->byteWeapon == WEAPON_MINIGUN)
        {
            if (Animation::IsProblematicAnimation(data->wAnimIndex)) {
                // Only remove action key if holding aim
                if (data->wKeys & 128) {
                    data->wKeys &= ~1; // Remove action key
                }
                data->wKeys &= ~4;   // Remove fire key
                data->wKeys &= ~128; // Remove aim key
            }
        }
        else if (data->byteWeapon == WEAPON_SPRAYCAN ||
                 data->byteWeapon == WEAPON_FIREEXTINGUISHER ||
                 data->byteWeapon == WEAPON_FLAMETHROWER)
        {
            if (data->wAnimIndex < Animation::IDs::SPRAYCAN_START ||
                data->wAnimIndex > Animation::IDs::SPRAYCAN_END)
            {
                if (data->wKeys & 128) {
                    data->wKeys &= ~1; // Remove action key
                }
                data->wKeys &= ~4;   // Remove fire key
                data->wKeys &= ~128; // Remove aim key
            }
        }
        else if (data->byteWeapon == WEAPON_GRENADE)
        {
            if (data->wAnimIndex < Animation::IDs::GRENADE_START ||
                data->wAnimIndex > Animation::IDs::GRENADE_END)
            {
                data->wKeys &= ~1; // Remove action key
            }
        }
    }

    template <typename SyncData>
    void ApplyFakePlayerStats(WORD playerId, SyncData *data)
    {
        if (Player::fakeHealth[playerId] != 255) {
            data->byteHealth = Player::fakeHealth[playerId];
        }

        if (Player::fakeArmour[playerId] != 255) {
            data->byteArmour = Player::fakeArmour[playerId];
        }
    }

    template <typename SyncData>
    void ApplyFakeVehicleStats(WORD playerId, SyncData *data)
    {
        if (Player::fakeHealth[playerId] != 255) {
            data->bytePlayerHealth = Player::fakeHealth[playerId];
        }

        if (Player::fakeArmour[playerId] != 255) {
            data->bytePlayerArmour = Player::fakeArmour[playerId];
        }
    }

    template <typename Structs>
    bool ProcessPlayerSync(Packet *packet, WORD playerId)
    {
        using SyncTypes = Global::SyncTypes;

        if (packet->length != (sizeof(typename Structs::CSyncData) + 1)) {
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

        if (!ValidateWeaponID(data->byteWeapon)) {
            data->byteWeapon = 0; // Set to unarmed
        }

        // Fix detonator crasher - removes AIM_KEY for detonator
        if (data->byteWeapon == 40) {
            data->wKeys &= ~128;
        }

        ApplyGhostShootingPrevention(data);

        SyncFreezing::HandlePlayerSyncFreezing<Structs>(playerId, data);

        if (Player::blockKeySync[playerId]) {
            data->wKeys = 0;
        }

        ApplyFakePlayerStats(playerId, data);

        // Apply fake quaternion if set
        if (Player::fakeQuat[playerId] != NULL) {
            data->fQuaternion[0] = Player::fakeQuat[playerId]->w; // angle
            data->fQuaternion[1] = Player::fakeQuat[playerId]->x; // x
            data->fQuaternion[2] = Player::fakeQuat[playerId]->y; // y
            data->fQuaternion[3] = Player::fakeQuat[playerId]->z; // z
        }

        // Handle special weapon key restrictions
        if (data->byteWeapon == 44 || data->byteWeapon == 45) {
            data->wKeys &= ~4; // Remove fire key for night vision/thermal goggles
        }
        else if (data->byteWeapon == 4 && !Global::knifeSync) {
            data->wKeys &= ~128; // Remove aim key for knife if knife sync disabled
        }

        Player::lastWeapon[playerId] = data->byteWeapon;
        Player::lastSyncPacket[playerId] = SyncTypes::E_PLAYER_SYNC;

        return true;
    }

    template <typename Structs>
    bool ProcessAimSync(Packet *packet, WORD playerId)
    {
        using SyncTypes = Global::SyncTypes;

        if (packet->length != (sizeof(typename Structs::CAimSyncData) + 1)) {
            return false;
        }

        auto *data = reinterpret_cast<typename Structs::CAimSyncData *>(&packet->data[1]);

        if (!ValidatePosition(data->vecFront) ||
            !ValidatePosition(data->vecPosition))
        {
            return false;
        }

        SyncFreezing::HandleAimSyncFreezing<Structs>(playerId, data);

        // Fix first-person up/down aim sync for sniper rifles
        if (Player::lastWeapon[playerId] == 34 ||
            Player::lastWeapon[playerId] == 35 ||
            Player::lastWeapon[playerId] == 36 ||
            Player::lastWeapon[playerId] == 43)
        {
            data->fZAim = -data->vecFront.fZ;
            data->fZAim = std::clamp(data->fZAim, -1.0f, 1.0f);
        }

        if (Player::infiniteAmmo[playerId]) {
            data->byteCameraZoom = 2;
        }

        Player::lastSyncPacket[playerId] = SyncTypes::E_AIM_SYNC;
        return true;
    }

    template <typename Structs>
    bool ProcessVehicleSync(Packet *packet, WORD playerId)
    {
        using SyncTypes = Global::SyncTypes;

        if (packet->length != (sizeof(typename Structs::CVehicleSyncData) + 1)) {
            return false;
        }

        auto *data = reinterpret_cast<typename Structs::CVehicleSyncData *>(&packet->data[1]);

        if (!ValidateVehicleID(data->wVehicleId)) {
            return false;
        }

        if (!ValidatePosition(data->vecPosition) ||
            !ValidatePosition(data->vecVelocity))
        {
            return false;
        }

        SyncFreezing::HandleVehicleSyncFreezing<Structs>(playerId, data);

        if (!ValidateWeaponID(data->bytePlayerWeapon)) {
            data->bytePlayerWeapon = 0; // Set to unarmed
        }

        ApplyFakeVehicleStats(playerId, data);

        Player::lastSyncPacket[playerId] = SyncTypes::E_VEHICLE_SYNC;
        return true;
    }

    template <typename Structs>
    bool ProcessPassengerSync(Packet *packet, WORD playerId)
    {
        using SyncTypes = Global::SyncTypes;

        if (packet->length != (sizeof(typename Structs::CPassengerSyncData) + 1)) {
            return false;
        }

        auto *data = reinterpret_cast<typename Structs::CPassengerSyncData *>(&packet->data[1]);

        if (!ValidateVehicleID(data->wVehicleId)) {
            return false;
        }

        if (!ValidatePosition(data->vecPosition)) {
            return false;
        }

        SyncFreezing::HandlePassengerSyncFreezing<Structs>(playerId, data);

        if (!ValidateWeaponID(data->bytePlayerWeapon)) {
            data->bytePlayerWeapon = 0; // Set to unarmed
        }

        ApplyFakeVehicleStats(playerId, data);

        Player::lastSyncPacket[playerId] = SyncTypes::E_PASSENGER_SYNC;
        return true;
    }

    template <typename Structs>
    bool ProcessSpectatorSync(Packet *packet, WORD playerId)
    {
        using SyncTypes = Global::SyncTypes;

        if (packet->length != (sizeof(typename Structs::CSpectatingSyncData) + 1)) {
            return false;
        }

        auto *data = reinterpret_cast<typename Structs::CSpectatingSyncData *>(&packet->data[1]);

        if (!ValidatePosition(data->vecPosition)) {
            return false;
        }

        SyncFreezing::HandleSpectatorSyncFreezing<Structs>(playerId, data);

        Player::lastSyncPacket[playerId] = SyncTypes::E_SPECTATING_SYNC;
        return true;
    }
}
