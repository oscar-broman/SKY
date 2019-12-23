/*
    SKY
*/

#pragma once

#include "CVector.h"
#include "CVector2D.h"
#include <map>

#include "CTypes.h"
#include "main.h"
#include "RPCs.h"
#include "Hooks.h"
#include <amx/amx.h>
#include <plugincommon.h>
#include <raknet/BitStream.h>
#include <raknet/NetworkTypes.h>

#if !defined PAD
#define PAD(a, b) char a[b]
#endif

struct without_dl
{
#include "Structs.h"
};

struct with_dl
{
#define SAMP_03DL
#include "Structs.h"
};

template <class Func>
auto getNetGame(Func func)
{
    if (iVersion == eSAMPVersion::SAMP_VERSION_03DL_R1)
    {
        static with_dl::CNetGame *netGame = reinterpret_cast<with_dl::CNetGame *>(pNetGame);
        return func(netGame, with_dl());
    }
    else
    {
        static without_dl::CNetGame *netGame = reinterpret_cast<without_dl::CNetGame *>(pNetGame);
        return func(netGame, without_dl());
    }
}

template <typename Struct>
auto &getLastSyncData(int playerid)
{
    static typename Struct::CSyncData data[MAX_PLAYERS];
    return data[playerid];
}

template <typename Struct>
auto &getLastAimSyncData(int playerid)
{
    static typename Struct::CAimSyncData data[MAX_PLAYERS];
    return data[playerid];
}

template <typename Struct>
auto &getLastVehicleSyncData(int playerid)
{
    static typename Struct::CVehicleSyncData data[MAX_PLAYERS];
    return data[playerid];
}

template <typename Struct>
auto &getLastPassengerSyncData(int playerid)
{
    static typename Struct::CPassengerSyncData data[MAX_PLAYERS];
    return data[playerid];
}

template <typename Struct>
void sendSyncData(int playerid, int animation, RakNet::BitStream *bs)
{
    auto *d = &getLastSyncData<Struct>(playerid);

    bs->Write((BYTE)ID_PLAYER_SYNC);
    bs->Write((WORD)playerid);

    if (d->wUDAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wUDAnalog);
    }
    else
    {
        bs->Write(false);
    }

    if (d->wLRAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wLRAnalog);
    }
    else
    {
        bs->Write(false);
    }

    bs->Write((WORD)d->wKeys);

    bs->Write(d->vecPosition.fX);
    bs->Write(d->vecPosition.fY);
    bs->Write(d->vecPosition.fZ);

    if (fakeQuat[playerid] != NULL)
    {
        bs->Write((bool)(fakeQuat[playerid]->w < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->x < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->y < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->z < 0.0f));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->x) * 65535.0));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->y) * 65535.0));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->z) * 65535.0));
    }
    else
    {
        bs->Write((bool)(d->fQuaternion[0] < 0.0f));
        bs->Write((bool)(d->fQuaternion[1] < 0.0f));
        bs->Write((bool)(d->fQuaternion[2] < 0.0f));
        bs->Write((bool)(d->fQuaternion[3] < 0.0f));
        bs->Write((unsigned short)(fabs(d->fQuaternion[1]) * 65535.0));
        bs->Write((unsigned short)(fabs(d->fQuaternion[2]) * 65535.0));
        bs->Write((unsigned short)(fabs(d->fQuaternion[3]) * 65535.0));
    }

    BYTE health, armour;

    if (fakeHealth[playerid] != 255)
    {
        health = fakeHealth[playerid];
    }
    else
    {
        health = d->byteHealth;
    }

    if (fakeArmour[playerid] != 255)
    {
        armour = fakeArmour[playerid];
    }
    else
    {
        armour = d->byteArmour;
    }

    if (health >= 100)
    {
        health = 0xF;
    }
    else
    {
        health /= 7;
    }

    if (armour >= 100)
    {
        armour = 0xF;
    }
    else
    {
        armour /= 7;
    }

    bs->Write((BYTE)((health << 4) | (armour)));

    bs->Write(d->_unk_);
    bs->Write(d->byteWeapon);
    bs->Write(d->byteSpecialAction);

    // Make them appear standing still if paused
    if (GetTickCount() - lastUpdateTick[playerid] > 2000)
    {
        bs->WriteVector(0.0f, 0.0f, 0.0f);
    }
    else
    {
        bs->WriteVector(d->vecVelocity.fX, d->vecVelocity.fY, d->vecVelocity.fZ);
    }

    if (d->wSurfingInfo)
    {
        bs->Write(true);

        bs->Write(d->wSurfingInfo);
        bs->Write(d->vecSurfing.fX);
        bs->Write(d->vecSurfing.fY);
        bs->Write(d->vecSurfing.fZ);
    }
    else
    {
        bs->Write(false);
    }

    // Animations are only sent when they are changed
    if (animation)
    {
        bs->Write(true);
        bs->Write(animation);
    }
    else
    {
        bs->Write(false);
    }    
}

template <typename Struct>
void sendAimSyncData(int playerid, RakNet::BitStream *bs)
{
    auto *d = &getLastAimSyncData<Struct>(playerid);

    bs->Write((BYTE)ID_AIM_SYNC);
    bs->Write((WORD)playerid);

    bs->Write((BYTE)d->byteCameraMode);

    bs->Write(d->vecFront.fX);
    bs->Write(d->vecFront.fY);
    bs->Write(d->vecFront.fZ);

    bs->Write(d->vecPosition.fX);
    bs->Write(d->vecPosition.fY);
    bs->Write(d->vecPosition.fZ);

    // Fix first-person up/down aim sync
    if (lastWeapon[playerid] == 34 || lastWeapon[playerid] == 35 || lastWeapon[playerid] == 36 || lastWeapon[playerid] == 43)
    {
        d->fZAim = -d->vecFront.fZ;

        if (d->fZAim > 1.0f)
        {
            d->fZAim = 1.0f;
        }
        else if (d->fZAim < -1.0f)
        {
            d->fZAim = -1.0f;
        }
    }    

    bs->Write(d->fZAim);

    if (infiniteAmmo[playerid])
    {
        d->byteCameraZoom = 2;
    }

    bs->Write((BYTE)d->byteCameraZoom);
    bs->Write((BYTE)d->byteWeaponState);    
    bs->Write((BYTE)d->byteAspectRatio);
}

template <typename Struct>
void sendVehicleSyncData(int playerid, RakNet::BitStream *bs)
{
    auto *d = &getLastVehicleSyncData<Struct>(playerid);

    bs->Write((BYTE)ID_VEHICLE_SYNC);
    bs->Write((WORD)playerid);
    bs->Write((WORD)d->wVehicleId);

    if (d->wUDAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wUDAnalog);
    }
    else
    {
        bs->Write(false);
    }

    if (d->wLRAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wLRAnalog);
    }
    else
    {
        bs->Write(false);
    }

    bs->Write((WORD)d->wKeys);

    if (fakeQuat[playerid] != NULL)
    {
        bs->Write((bool)(fakeQuat[playerid]->w < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->x < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->y < 0.0f));
        bs->Write((bool)(fakeQuat[playerid]->z < 0.0f));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->x) * 65535.0));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->y) * 65535.0));
        bs->Write((unsigned short)(fabs(fakeQuat[playerid]->z) * 65535.0));
    }
    else
    {
        bs->Write((bool)(d->fQuaternion[0] < 0.0f));
        bs->Write((bool)(d->fQuaternion[1] < 0.0f));
        bs->Write((bool)(d->fQuaternion[2] < 0.0f));
        bs->Write((bool)(d->fQuaternion[3] < 0.0f));
        bs->Write((unsigned short)(fabs(d->fQuaternion[1]) * 65535.0));
        bs->Write((unsigned short)(fabs(d->fQuaternion[2]) * 65535.0));
        bs->Write((unsigned short)(fabs(d->fQuaternion[3]) * 65535.0));
    }

    bs->WriteVector(d->vecPosition.fX, d->vecPosition.fY, d->vecPosition.fZ);
    bs->WriteVector(d->vecVelocity.fX, d->vecVelocity.fY, d->vecVelocity.fZ);
    bs->Write(d->fHealth);

    BYTE health, armour;
    if (fakeHealth[playerid] != 255)
    {
        health = fakeHealth[playerid];
    }
    else
    {
        health = d->bytePlayerHealth;
    }

    if (fakeArmour[playerid] != 255)
    {
        armour = fakeArmour[playerid];
    }
    else
    {
        armour = d->bytePlayerArmour;
    }

    if (health >= 100)
    {
        health = 0xF;
    }
    else
    {
        health /= 7;
    }

    if (armour >= 100)
    {
        armour = 0xF;
    }
    else
    {
        armour /= 7;
    }

    bs->Write((BYTE)((health << 4) | (armour)));

    bs->Write(d->unk_2); // Additional Keys
    bs->Write(d->bytePlayerWeapon);

    if(d->byteSirenState)
    {
        bs->Write(true);
    } 
    else 
    {
        bs->Write(false);
    }

    if(d->byteGearState)
    {
        bs->Write(true);
    } 
    else 
    {
        bs->Write(false);
    }

    if(d->fTrainSpeed)
    {
        bs->Write(true);
        bs->Write(d->fTrainSpeed);
    }
    else 
    {
        bs->Write(false);
    }

    if(d->wTrailerID)
    {
        bs->Write(true);
        bs->Write(d->wTrailerID);
    }
    else 
    {
        bs->Write(false);
    }
}

template <typename Struct>
void sendPassengerSyncData(int playerid, RakNet::BitStream *bs)
{
    auto *d = &getLastPassengerSyncData<Struct>(playerid);

    bs->Write((BYTE)ID_PASSENGER_SYNC);
    bs->Write((WORD)playerid);
    bs->Write((WORD)d->wVehicleId);

    bs->Write(d->byteDriveBy);
    bs->Write(d->byteSeatFlags);

    bs->Write(d->byteAdditionalKeys);
    bs->Write(d->bytePlayerWeapon);

    BYTE health, armour;
    if (fakeHealth[playerid] != 255)
    {
        health = fakeHealth[playerid];
    }
    else
    {
        health = d->bytePlayerHealth;
    }

    if (fakeArmour[playerid] != 255)
    {
        armour = fakeArmour[playerid];
    }
    else
    {
        armour = d->bytePlayerArmour;
    }

    if (health >= 100)
    {
        health = 0xF;
    }
    else
    {
        health /= 7;
    }

    if (armour >= 100)
    {
        armour = 0xF;
    }
    else
    {
        armour /= 7;
    }

    bs->Write((BYTE)((health << 4) | (armour)));

    if (d->wUDAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wUDAnalog);
    }
    else
    {
        bs->Write(false);
    }

    if (d->wLRAnalog)
    {
        bs->Write(true);
        bs->Write((WORD)d->wLRAnalog);
    }
    else
    {
        bs->Write(false);
    }

    bs->Write((WORD)d->wKeys);

    bs->WriteVector(d->vecPosition.fX, d->vecPosition.fY, d->vecPosition.fZ);
}