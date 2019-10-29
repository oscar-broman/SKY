/*
    SKY
*/

#pragma once

#include "CVector.h"
#include "CVector2D.h"
#include <map>

#include "CTypes.h"
#include "main.h"
#include <amx/amx.h>
#include <raknet/BitStream.h>
#include <raknet/NetworkTypes.h>
#include <samp-plugin-sdk/plugincommon.h>

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
        auto netGame = reinterpret_cast<with_dl::CNetGame *>(pNetGame);
        return func(netGame, with_dl());
    }
    else
    {
        auto netGame = reinterpret_cast<without_dl::CNetGame *>(pNetGame);
        return func(netGame, without_dl());
    }
}

template <class NetGame>
auto &getLastSyncData(int playerid)
{
    static decltype(NetGame()->pPlayerPool->pPlayer[playerid]->syncData) data[MAX_PLAYERS];
    return data[playerid];
}