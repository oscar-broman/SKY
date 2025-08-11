#pragma once

#include "../structs/CTypes.h"
#include "../core/Global.h"

namespace SyncFreezing
{
    template <typename Structs, typename SyncType>
    void HandlePlayerSyncFreezing(WORD playerId, typename Structs::CSyncData *data, SyncType syncType);

    template <typename Structs, typename SyncType>
    void HandleAimSyncFreezing(WORD playerId, typename Structs::CAimSyncData *data, SyncType syncType);

    template <typename Structs, typename SyncType>
    void HandleVehicleSyncFreezing(WORD playerId, typename Structs::CVehicleSyncData *data, SyncType syncType);

    template <typename Structs, typename SyncType>
    void HandlePassengerSyncFreezing(WORD playerId, typename Structs::CPassengerSyncData *data, SyncType syncType);

    template <typename Structs, typename SyncType>
    void HandleSpectatorSyncFreezing(WORD playerId, typename Structs::CSpectatingSyncData *data, SyncType syncType);
}

// Template implementations
#include "SyncFreezing.inl"
