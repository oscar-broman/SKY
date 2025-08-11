#pragma once

#include "../structs/CTypes.h"
#include "../core/Global.h"

namespace SyncFreezing
{
    template <typename Structs>
    void HandlePlayerSyncFreezing(WORD playerId, typename Structs::CSyncData *data);

    template <typename Structs>
    void HandleAimSyncFreezing(WORD playerId, typename Structs::CAimSyncData *data);

    template <typename Structs>
    void HandleVehicleSyncFreezing(WORD playerId, typename Structs::CVehicleSyncData *data);

    template <typename Structs>
    void HandlePassengerSyncFreezing(WORD playerId, typename Structs::CPassengerSyncData *data);

    template <typename Structs>
    void HandleSpectatorSyncFreezing(WORD playerId, typename Structs::CSpectatingSyncData *data);
}

// Template implementations
#include "SyncFreezing.inl"
