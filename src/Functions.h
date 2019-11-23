/*
*  Version: MPL 1.1
*
*  The contents of this file are subject to the Mozilla Public License Version
*  1.1 (the "License"); you may not use this file except in compliance with
*  the License. You may obtain a copy of the License at
*  http://www.mozilla.org/MPL/
*
*  Software distributed under the License is distributed on an "AS IS" basis,
*  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
*  for the specific language governing rights and limitations under the
*  License.
*
*  The Original Code is the YSI 2.0 SA:MP plugin.
*
*  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
*  Portions created by the Initial Developer are Copyright (C) 2008
*  the Initial Developer. All Rights Reserved. The development was abandobed
*  around 2010, afterwards kurta999 has continued it.
*
*  Contributor(s):
*
*	0x688, balika011, Gamer_Z, iFarbod, karimcambridge, Mellnik, P3ti, Riddick94
*	Slice, sprtik, uint32, Whitetigerswt, Y_Less, ziggi and complete SA-MP community
*
*  Special Thanks to:
*
*	SA:MP Team past, present and future
*	Incognito, maddinat0r, OrMisicL, Zeex
*
*/

#ifndef YSF_FUNCTIONS_H
#define YSF_FUNCTIONS_H

#include "main.h"
#include "net.h"
#include <raknet/NetworkTypes.h>

#define DEFINE_FUNCTION_POINTER(name) \
	static name ## _t pfn__ ## name

#define POINT_TO_MEMBER(name, address) \
	pfn__ ## name = (name ## _t)(address)

#define INIT_FPTR(name) \
	pfn__ ## name = (name ## _t)(CAddress::FUNC_ ## name)

#ifdef _WIN32
#define STDCALL __stdcall
#define THISCALL __thiscall
#define FASTCALL __fastcall
#else
#define STDCALL
#define THISCALL
#define FASTCALL
#define CDECL
#endif

struct Packet;
struct ConsoleVariable_s;

typedef void(THISCALL *CPlayer__SpawnForWorld_t)(void *pPlayer);

typedef bool(THISCALL *RakNet__Send_t)(void *ppRakServer, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);
typedef Packet *(THISCALL *RakNet__Receive_t)(void *ppRakServer);
typedef bool(THISCALL *RakNet__RPC_t)(void *ppRakServer, int *uniqueID, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
typedef PlayerID(THISCALL *RakNet__GetPlayerIDFromIndex_t)(void *ppRakServer, int index);

class CHookRakServer
{
public:
	static Packet *THISCALL Receive(void *ppRakServer);
};

class CSAMPFunctions
{
public:
	static void Initialize();

	static void SpawnPlayer(int iPlayerId);

	static bool Send(RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);
	static Packet *Receive(void *ppRakServer);
	static bool RPC(int *uniqueID, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
	static PlayerID GetPlayerIDFromIndex(int index);

	// RakServer
	DEFINE_FUNCTION_POINTER(RakNet__Send);
	DEFINE_FUNCTION_POINTER(RakNet__Receive);
	DEFINE_FUNCTION_POINTER(RakNet__RPC);
	DEFINE_FUNCTION_POINTER(RakNet__GetPlayerIDFromIndex);
};

#endif
