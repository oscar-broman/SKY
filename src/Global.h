#ifndef SKY_GLOBAL
#define SKY_GLOBAL

#include "CTypes.h"

namespace Global
{
    enum SyncTypes
    {
        E_PLAYER_SYNC = 0,
        E_AIM_SYNC,
        E_VEHICLE_SYNC,
        E_PASSENGER_SYNC,
        E_SPECTATING_SYNC,
        E_LAST_SYNC,
        E_ALL_SYNC
    };    

    extern BOOL disableSyncBugs;
    extern BOOL knifeSync;
};

#endif