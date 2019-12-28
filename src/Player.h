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
};

#endif