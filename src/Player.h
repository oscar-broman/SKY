#ifndef SKY_PLAYER
#define SKY_PLAYER

#include "CTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Global.h"

#ifndef MAX_PLAYERS
    #define MAX_PLAYERS 1000
#endif

namespace Player
{
    using SyncTypes = Global::SyncTypes;

    extern BYTE lastWeapon[MAX_PLAYERS];
    extern BYTE fakeHealth[MAX_PLAYERS];
    extern BYTE fakeArmour[MAX_PLAYERS];    
    extern glm::quat *fakeQuat[MAX_PLAYERS];
    extern DWORD lastUpdateTick[MAX_PLAYERS];
    extern BOOL blockKeySync[MAX_PLAYERS];
    extern BOOL infiniteAmmo[MAX_PLAYERS];

    extern BOOL syncDataFrozen[MAX_PLAYERS];
    extern BOOL syncAimDataFrozen[MAX_PLAYERS];
    extern BOOL syncVehicleDataFrozen[MAX_PLAYERS];
    extern BOOL syncPassengerDataFrozen[MAX_PLAYERS];
    extern BOOL syncSpectatingDataFrozen[MAX_PLAYERS];

    extern Global::SyncTypes lastSyncPacket[MAX_PLAYERS];

    extern void SetSyncFrozenState(int playerid, SyncTypes type, bool toggle);
    extern BOOL GetSyncFrozenState(int playerid, SyncTypes type);

    // Returns the last sync data stored for the struct CSyncData
    template <typename Struct>
    auto &getLastSyncData(int playerid)
    {
        static typename Struct::CSyncData data[MAX_PLAYERS];
        return data[playerid];
    }

    // Returns the last sync data stored for the struct CAimSyncData
    template <typename Struct>
    auto &getLastAimSyncData(int playerid)
    {
        static typename Struct::CAimSyncData data[MAX_PLAYERS];
        return data[playerid];
    }

    // Returns the last sync data stored for the struct CVehicleSyncData
    template <typename Struct>
    auto &getLastVehicleSyncData(int playerid)
    {
        static typename Struct::CVehicleSyncData data[MAX_PLAYERS];
        return data[playerid];
    }

    // Returns the last sync data stored for the struct CPassengerSyncData
    template <typename Struct>
    auto &getLastPassengerSyncData(int playerid)
    {
        static typename Struct::CPassengerSyncData data[MAX_PLAYERS];
        return data[playerid];
    }

    // Returns the last sync data stored for the struct CSpectatingSyncData
    template <typename Struct>
    auto &getLastSpectatingSyncData(int playerid)
    {
        static typename Struct::CSpectatingSyncData data[MAX_PLAYERS];
        return data[playerid];
    }            
};

#endif