#ifndef SKY_PLAYER
#define SKY_PLAYER

#include "global.hpp"
#include "values.hpp"
#include "types.hpp"
#include "Shared/netcode.hpp"

namespace Player
{
    inline int lastWeapon[PLAYER_POOL_SIZE] = {0};
    inline float fakeHealth[PLAYER_POOL_SIZE] = {0};
    inline float fakeArmour[PLAYER_POOL_SIZE] = {0};
    inline Vector4 *fakeQuat[PLAYER_POOL_SIZE];
    inline unsigned int lastUpdateTick[PLAYER_POOL_SIZE] = {0};
    inline bool blockKeySync[PLAYER_POOL_SIZE] = {0};
    inline bool infiniteAmmo[PLAYER_POOL_SIZE] = {0};

    inline bool syncOnFootDataFrozen[PLAYER_POOL_SIZE] = {0};     // Stores the frozen state for OnFoot Sync
    inline bool syncAimDataFrozen[PLAYER_POOL_SIZE] = {0};        // Stores the frozen state for Aim Sync
    inline bool syncVehicleDataFrozen[PLAYER_POOL_SIZE] = {0};    // Stores the frozen state for Vehicle Sync
    inline bool syncPassengerDataFrozen[PLAYER_POOL_SIZE] = {0};  // Stores the frozen state for Passenger Sync
    inline bool syncSpectatingDataFrozen[PLAYER_POOL_SIZE] = {0}; // Stores the frozen state for Spectating Sync
    inline bool syncAllDataFrozen[PLAYER_POOL_SIZE] = {0};

    inline SyncTypes lastSyncPacket[PLAYER_POOL_SIZE] = {SyncTypes::E_PLAYER_SYNC}; // Stores the last packets type

    inline NetCode::Packet::PlayerFootSync lastSyncData[PLAYER_POOL_SIZE];
    inline NetCode::Packet::PlayerVehicleSync lastVehicleSyncData[PLAYER_POOL_SIZE];
    inline NetCode::Packet::PlayerAimSync lastPlayerAimSyncData[PLAYER_POOL_SIZE];
    inline NetCode::Packet::PlayerPassengerSync lastPlayerPassengerSyncData[PLAYER_POOL_SIZE];
    inline NetCode::Packet::PlayerSpectatorSync lastPlayerSpectateSyncData[PLAYER_POOL_SIZE];
    inline NetCode::Packet::PlayerTrailerSync lastPlayerTrailerSyncData[PLAYER_POOL_SIZE];

    void SetSyncFrozenState(int playerid, SyncTypes type, bool toggle);
    bool GetSyncFrozenState(int playerid, SyncTypes type);
};

#endif