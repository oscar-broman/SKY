#pragma once

#include "Player.h"
#include <cstring>

namespace SyncFreezing
{
    template <typename Structs>
    void HandlePlayerSyncFreezing(WORD playerId, typename Structs::CSyncData *data)
    {
        using SyncTypes = Global::SyncTypes;

        auto lastSyncData = &Player::getLastSyncData<Structs>(playerId);

        if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PLAYER_SYNC) ||
            Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
        {
            std::memcpy(data, lastSyncData, sizeof(typename Structs::CSyncData));
        }
        else
        {
            std::memcpy(lastSyncData, data, sizeof(typename Structs::CSyncData));
        }
    }

    template <typename Structs>
    void HandleAimSyncFreezing(WORD playerId, typename Structs::CAimSyncData *data)
    {
        using SyncTypes = Global::SyncTypes;

        auto lastAimSyncData = &Player::getLastAimSyncData<Structs>(playerId);

        if (Player::GetSyncFrozenState(playerId, SyncTypes::E_AIM_SYNC) ||
            Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
        {
            std::memcpy(data, lastAimSyncData, sizeof(typename Structs::CAimSyncData));
        }
        else
        {
            std::memcpy(lastAimSyncData, data, sizeof(typename Structs::CAimSyncData));
        }
    }

    template <typename Structs>
    void HandleVehicleSyncFreezing(WORD playerId, typename Structs::CVehicleSyncData *data)
    {
        using SyncTypes = Global::SyncTypes;

        auto lastVehicleSyncData = &Player::getLastVehicleSyncData<Structs>(playerId);

        if (Player::GetSyncFrozenState(playerId, SyncTypes::E_VEHICLE_SYNC) ||
            Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
        {
            std::memcpy(data, lastVehicleSyncData, sizeof(typename Structs::CVehicleSyncData));
        }
        else
        {
            std::memcpy(lastVehicleSyncData, data, sizeof(typename Structs::CVehicleSyncData));
        }
    }

    template <typename Structs>
    void HandlePassengerSyncFreezing(WORD playerId, typename Structs::CPassengerSyncData *data)
    {
        using SyncTypes = Global::SyncTypes;

        auto lastPassengerSyncData = &Player::getLastPassengerSyncData<Structs>(playerId);

        if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PASSENGER_SYNC) ||
            Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
        {
            std::memcpy(data, lastPassengerSyncData, sizeof(typename Structs::CPassengerSyncData));
        }
        else
        {
            std::memcpy(lastPassengerSyncData, data, sizeof(typename Structs::CPassengerSyncData));
        }
    }

    template <typename Structs>
    void HandleSpectatorSyncFreezing(WORD playerId, typename Structs::CSpectatingSyncData *data)
    {
       using SyncTypes = Global::SyncTypes;

        auto lastSpectatingSyncData = &Player::getLastSpectatingSyncData<Structs>(playerId);

        if (Player::GetSyncFrozenState(playerId, SyncTypes::E_SPECTATING_SYNC) ||
            Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
        {
            std::memcpy(data, lastSpectatingSyncData, sizeof(typename Structs::CSpectatingSyncData));
        }
        else
        {
            std::memcpy(lastSpectatingSyncData, data, sizeof(typename Structs::CSpectatingSyncData));
        }
    }
}
