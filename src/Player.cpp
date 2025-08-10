#include "Player.h"
#include "RPCs.h"

#ifdef _WIN32
#include <windows.h>
#endif
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

    PacketRateLimit packetRateLimit[MAX_PLAYERS];  // Rate limiter for all packets

    void InitializeRateLimits(int playerid)
    {
        if (playerid < 0 || playerid >= MAX_PLAYERS)
            return;

        DWORD currentTime = GetTickCount();

        // Packet rate limiter: 200 packets per second (allows normal gameplay while preventing floods)
        packetRateLimit[playerid] = {currentTime, 0, 200, 1000};
    }

    void ResetRateLimits(int playerid)
    {
        InitializeRateLimits(playerid);
    }

    bool CheckPacketRateLimit(int playerid)
    {
        if (playerid < 0 || playerid >= MAX_PLAYERS)
            return false;

        PacketRateLimit *rateLimit = &packetRateLimit[playerid];
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