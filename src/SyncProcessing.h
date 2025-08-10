#pragma once

#include "CTypes.h"
#include "Global.h"

namespace SyncProcessing
{
    template <typename SyncData>
    void ApplyGhostShootingPrevention(SyncData *data);

    template <typename SyncData>
    void ApplyFakePlayerStats(WORD playerId, SyncData *data);

    template <typename SyncData>
    void ApplyFakeVehicleStats(WORD playerId, SyncData *data);

    template <typename Structs>
    bool ProcessPlayerSync(Packet *packet, WORD playerId);

    template <typename Structs>
    bool ProcessAimSync(Packet *packet, WORD playerId);

    template <typename Structs>
    bool ProcessVehicleSync(Packet *packet, WORD playerId);

    template <typename Structs>
    bool ProcessPassengerSync(Packet *packet, WORD playerId);

    template <typename Structs>
    bool ProcessSpectatorSync(Packet *packet, WORD playerId);
}

// Template implementations
#include "SyncProcessing.inl"
