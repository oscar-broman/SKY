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
#include "Player.h"
#include <amx/amx.h>
#include <plugincommon.h>
#include <raknet/BitStream.h>
#include <raknet/NetworkTypes.h>

#if !defined PAD
    #define PAD(a, b) char a[b]
#endif

namespace Versions
{

    struct samp_037
    {
        #include "Structs.h"
    };

    struct samp_03dl
    {
        #define SAMP_03DL
        #include "Structs.h"
    };

    // Returns a function with an instance of netGame and a struct
    // which is either samp_037 or samp_03dl
    template <class Func>
    auto getNetGame(Func func)
    {
        if (iVersion == eSAMPVersion::SAMP_VERSION_03DL_R1)
        {
            static samp_03dl::CNetGame *netGame = reinterpret_cast<samp_03dl::CNetGame *>(pNetGame);
            return func(netGame, samp_03dl());
        }
        else
        {
            static samp_037::CNetGame *netGame = reinterpret_cast<samp_037::CNetGame *>(pNetGame);
            return func(netGame, samp_037());
        }
    }  

    // Writes a CSyncData struct to a bitstream instance
    template <typename Struct>
    void sendSyncData(int playerid, int animation, RakNet::BitStream *bs)
    {
        auto *d = &Player::getLastSyncData<Struct>(playerid);

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

        if (Player::fakeQuat[playerid] != NULL)
        {
            bs->Write((bool)(Player::fakeQuat[playerid]->w < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->x < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->y < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->z < 0.0f));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->x) * 65535.0));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->y) * 65535.0));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->z) * 65535.0));
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

        if (Player::fakeHealth[playerid] != 255)
        {
            health = Player::fakeHealth[playerid];
        }
        else
        {
            health = d->byteHealth;
        }

        if (Player::fakeArmour[playerid] != 255)
        {
            armour = Player::fakeArmour[playerid];
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

        bs->Write((BYTE)((d->byteAdditionalKeys << 6) | (d->byteWeapon)));
        bs->Write(d->byteSpecialAction);

        // Make them appear standing still if paused
        if (GetTickCount() - Player::lastUpdateTick[playerid] > 2000)
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
            bs->WriteVector(d->vecSurfing.fX, d->vecSurfing.fY, d->vecSurfing.fZ);
        }
        else
        {
            bs->Write(false);
        }

        // Animations are only sent when they are changed
        if (animation )
        {
            bs->Write(true);
            bs->Write((DWORD)d->wAnimIndex);
            bs->Write((DWORD)d->wAnimFlags);
        }
        else
        {
            bs->Write(false);
        }    
    }

    // Writes a CSyncAimData struct to a bitstream instance
    template <typename Struct>
    void sendAimSyncData(int playerid, RakNet::BitStream *bs)
    {
        auto *d = &Player::getLastAimSyncData<Struct>(playerid);

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
        if (Player::lastWeapon[playerid] == 34 
        || Player::lastWeapon[playerid] == 35 
        || Player::lastWeapon[playerid] == 36 
        || Player::lastWeapon[playerid] == 43)
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

        if (Player::infiniteAmmo[playerid])
        {
            d->byteCameraZoom = 2;
        }

        bs->Write((BYTE)d->byteCameraZoom);
        bs->Write((BYTE)d->byteWeaponState);    
        bs->Write((BYTE)d->byteAspectRatio);
    }

    // Writes a CSyncVehicleData struct to a bitstream instance
    template <typename Struct>
    void sendVehicleSyncData(int playerid, RakNet::BitStream *bs)
    {
        auto *d = &Player::getLastVehicleSyncData<Struct>(playerid);

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

        if (Player::fakeQuat[playerid] != NULL)
        {
            bs->Write((bool)(Player::fakeQuat[playerid]->w < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->x < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->y < 0.0f));
            bs->Write((bool)(Player::fakeQuat[playerid]->z < 0.0f));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->x) * 65535.0));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->y) * 65535.0));
            bs->Write((unsigned short)(fabs(Player::fakeQuat[playerid]->z) * 65535.0));
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
        if (Player::fakeHealth[playerid] != 255)
        {
            health = Player::fakeHealth[playerid];
        }
        else
        {
            health = d->bytePlayerHealth;
        }

        if (Player::fakeArmour[playerid] != 255)
        {
            armour = Player::fakeArmour[playerid];
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

        bs->Write((BYTE)(d->bytePlayerWeapon << 2) | (d->byteAdditionalKeys));

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

    // Writes a CSyncPassengerData struct to a bitstream instance
    template <typename Struct>
    void sendPassengerSyncData(int playerid, RakNet::BitStream *bs)
    {
        auto *d = &Player::getLastPassengerSyncData<Struct>(playerid);

        bs->Write((BYTE)ID_PASSENGER_SYNC);
        bs->Write((WORD)playerid);
        bs->Write((WORD)d->wVehicleId);

        bs->Write(d->byteDriveBy);
        bs->Write(d->byteSeatFlags);

        bs->WriteBits((unsigned char *)d->byteAdditionalKeys, 2);
        bs->WriteBits((unsigned char *)d->bytePlayerWeapon, 6);

        BYTE health, armour;
        if (Player::fakeHealth[playerid] != 255)
        {
            health = Player::fakeHealth[playerid];
        }
        else
        {
            health = d->bytePlayerHealth;
        }

        if (Player::fakeArmour[playerid] != 255)
        {
            armour = Player::fakeArmour[playerid];
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

    // Writes a CSpectatingSyncData struct to a bitstream instance
    template <typename Struct>
    void sendSpectatingSyncData(int playerid, RakNet::BitStream *bs)
    {
        auto *d = &Player::getLastSpectatingSyncData<Struct>(playerid);

        bs->Write((BYTE)ID_SPECTATOR_SYNC);
        bs->Write((WORD)playerid);

        if (d->wLeftRightKeysOnSpectating)
        {
            bs->Write(true);
            bs->Write((WORD)d->wLeftRightKeysOnSpectating);
        }
        else
        {
            bs->Write(false);
        }

        if (d->wUpDownKeysOnSpectating)
        {
            bs->Write(true);
            bs->Write((WORD)d->wUpDownKeysOnSpectating);
        }
        else
        {
            bs->Write(false);
        }

        bs->Write((WORD)d->wKeysOnSpectating);

        bs->WriteVector(d->vecPosition.fX, d->vecPosition.fY, d->vecPosition.fZ);        
    }       
};