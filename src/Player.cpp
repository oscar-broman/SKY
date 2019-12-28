#include "Player.h"

namespace Player
{
    BYTE lastWeapon[1000] = {0};
    BYTE fakeHealth[1000] = {0};
    BYTE fakeArmour[1000] = {0};    
    glm::quat *fakeQuat[1000];
    DWORD lastUpdateTick[1000] = {0};
    BOOL blockKeySync[1000] = {0};
    BOOL infiniteAmmo[1000] = {0};

    BOOL syncDataFrozen[1000] = {0};
    BOOL syncAimDataFrozen[1000] = {0};
    BOOL syncVehicleDataFrozen[1000] = {0};
    BOOL syncPassengerDataFrozen[1000] = {0};
    BOOL syncSpectatingDataFrozen[1000] = {0};

    extern Global::SyncTypes lastSyncPacket[1000] = {Global::SyncTypes::E_PLAYER_SYNC};         
};