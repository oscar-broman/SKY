#include "player.hpp"

namespace Player
{
    // Set the state of the sync data
    void SetSyncFrozenState(int playerid, SyncTypes type, bool toggle)
    {
        switch (type)
        {
        case SyncTypes::E_PLAYER_SYNC:
            syncOnFootDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_AIM_SYNC:
            syncAimDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_VEHICLE_SYNC:
            syncVehicleDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_PASSENGER_SYNC:
            syncPassengerDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_SPECTATING_SYNC:
            syncSpectatingDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_ALL_SYNC:
            syncAllDataFrozen[playerid] = toggle;
            break;

        case SyncTypes::E_LAST_SYNC:
            break;
        }
    }

    // Get the state of the sync data
    bool GetSyncFrozenState(int playerid, SyncTypes type)
    {
        switch (type)
        {
        case SyncTypes::E_PLAYER_SYNC:
            return syncOnFootDataFrozen[playerid];

        case SyncTypes::E_AIM_SYNC:
            return syncAimDataFrozen[playerid];

        case SyncTypes::E_VEHICLE_SYNC:
            return syncVehicleDataFrozen[playerid];

        case SyncTypes::E_PASSENGER_SYNC:
            return syncPassengerDataFrozen[playerid];

        case SyncTypes::E_SPECTATING_SYNC:
            return syncSpectatingDataFrozen[playerid];

        case SyncTypes::E_ALL_SYNC:
            return syncAllDataFrozen[playerid];

        case SyncTypes::E_LAST_SYNC:
            break;
        }

        return -1;
    }
};