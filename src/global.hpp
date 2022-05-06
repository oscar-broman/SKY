#ifndef SKY_GLOBAL
#define SKY_GLOBAL

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

inline bool disableSyncBugs = false;
inline bool knifeSync = false;

#endif