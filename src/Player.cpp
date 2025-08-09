#include "Player.h"
#include "RPCs.h"
#include "Utils.h"

namespace Player
{
    BYTE lastWeapon[MAX_PLAYERS] = {0};
    BYTE fakeHealth[MAX_PLAYERS] = {0};
    BYTE fakeArmour[MAX_PLAYERS] = {0};
    glm::quat *fakeQuat[MAX_PLAYERS] = {0};
    DWORD lastUpdateTick[MAX_PLAYERS] = {0};
    BOOL blockKeySync[MAX_PLAYERS] = {0};
    BOOL infiniteAmmo[MAX_PLAYERS] = {0};

    BOOL syncOnFootDataFrozen[MAX_PLAYERS] = {0};     // Stores the frozen state for OnFoot Sync
    BOOL syncAimDataFrozen[MAX_PLAYERS] = {0};        // Stores the frozen state for Aim Sync
    BOOL syncVehicleDataFrozen[MAX_PLAYERS] = {0};    // Stores the frozen state for Vehicle Sync
    BOOL syncPassengerDataFrozen[MAX_PLAYERS] = {0};  // Stores the frozen state for Passenger Sync
    BOOL syncSpectatingDataFrozen[MAX_PLAYERS] = {0}; // Stores the frozen state for Spectating Sync
    BOOL syncAllDataFrozen[MAX_PLAYERS] = {0};

    extern SyncTypes lastSyncPacket[MAX_PLAYERS] = {SyncTypes::E_PLAYER_SYNC}; // Stores the last packets type

    PacketRateLimit playerSyncRateLimit[MAX_PLAYERS];     // 20 packets per 1000ms (normal gameplay)
    PacketRateLimit vehicleSyncRateLimit[MAX_PLAYERS];    // 15 packets per 1000ms (vehicle movement)
    PacketRateLimit aimSyncRateLimit[MAX_PLAYERS];        // 30 packets per 1000ms (aiming can be frequent)
    PacketRateLimit passengerSyncRateLimit[MAX_PLAYERS];  // 10 packets per 1000ms (passenger updates)
    PacketRateLimit spectatorSyncRateLimit[MAX_PLAYERS];  // 10 packets per 1000ms (spectating)
    PacketRateLimit unoccupiedSyncRateLimit[MAX_PLAYERS]; // 5 packets per 1000ms (unoccupied vehicles)
    PacketRateLimit trailerSyncRateLimit[MAX_PLAYERS];    // 5 packets per 1000ms (trailer sync)

    void InitializeRateLimits(int playerid)
    {
        if (playerid < 0 || playerid >= MAX_PLAYERS)
            return;

        DWORD currentTime = GetTickCount();

        // Player sync: 20 packets per second (normal movement/actions)
        playerSyncRateLimit[playerid] = {currentTime, 0, 20, 1000};

        // Vehicle sync: 15 packets per second (vehicle movement)
        vehicleSyncRateLimit[playerid] = {currentTime, 0, 15, 1000};

        // Aim sync: 30 packets per second (aiming can be frequent during combat)
        aimSyncRateLimit[playerid] = {currentTime, 0, 30, 1000};

        // Passenger sync: 10 packets per second (passenger actions)
        passengerSyncRateLimit[playerid] = {currentTime, 0, 10, 1000};

        // Spectator sync: 10 packets per second (spectating movement)
        spectatorSyncRateLimit[playerid] = {currentTime, 0, 10, 1000};

        // Unoccupied sync: 5 packets per second (unoccupied vehicle updates)
        unoccupiedSyncRateLimit[playerid] = {currentTime, 0, 5, 1000};

        // Trailer sync: 5 packets per second (trailer updates)
        trailerSyncRateLimit[playerid] = {currentTime, 0, 5, 1000};
    }

    void ResetRateLimits(int playerid)
    {
        InitializeRateLimits(playerid);
    }

    bool CheckPacketRateLimit(int playerid, unsigned char packetId)
    {
        if (playerid < 0 || playerid >= MAX_PLAYERS)
            return false;

        PacketRateLimit *rateLimit = nullptr;

        switch (packetId)
        {
        case ID_PLAYER_SYNC:
            rateLimit = &playerSyncRateLimit[playerid];
            break;
        case ID_VEHICLE_SYNC:
            rateLimit = &vehicleSyncRateLimit[playerid];
            break;
        case ID_AIM_SYNC:
            rateLimit = &aimSyncRateLimit[playerid];
            break;
        case ID_PASSENGER_SYNC:
            rateLimit = &passengerSyncRateLimit[playerid];
            break;
        case ID_SPECTATOR_SYNC:
            rateLimit = &spectatorSyncRateLimit[playerid];
            break;
        case ID_UNOCCUPIED_SYNC:
            rateLimit = &unoccupiedSyncRateLimit[playerid];
            break;
        case ID_TRAILER_SYNC:
            rateLimit = &trailerSyncRateLimit[playerid];
            break;
        default:
            // Unknown packet type, allow through
            return true;
        }

        if (rateLimit == nullptr)
            return true;

        DWORD currentTime = GetTickCount();

        if (currentTime - rateLimit->lastPacketTime >= rateLimit->timeWindowMs)
        {
            rateLimit->lastPacketTime = currentTime;
            rateLimit->packetCount = 1;
            return true;
        }
        else
        {
            rateLimit->packetCount++;

            if (rateLimit->packetCount > rateLimit->maxPacketsPerWindow)
            {
                return false;
            }

            return true;
        }
    }

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
        }
    }

    // Get the state of the sync data
    BOOL GetSyncFrozenState(int playerid, Global::SyncTypes type)
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
        }

        return -1;
    }
};