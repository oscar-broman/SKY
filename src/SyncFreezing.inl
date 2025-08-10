#pragma once

#include "Player.h"
#include <cstring>

namespace SyncFreezing
{
    template <typename Structs, typename SyncType>
    void HandlePlayerSyncFreezing(WORD playerId, typename Structs::CSyncData *data, SyncType syncType)
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
    void HandleAimSyncFreezing(WORD playerId, typename Structs::CAimSyncData *data, SyncType syncType)
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
    void HandleVehicleSyncFreezing(WORD playerId, typename Structs::CVehicleSyncData *data, SyncType syncType)
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
    void HandlePassengerSyncFreezing(WORD playerId, typename Structs::CPassengerSyncData *data, SyncType syncType)
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
    void HandleSpectatorSyncFreezing(WORD playerId, typename Structs::CSpectatingSyncData *data, SyncType syncType)
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
}
