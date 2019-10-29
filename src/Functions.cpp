#include <unordered_map>

#include "Addresses.h"
#include "Functions.h"
#include "Hooks.h"
#include "Versions.h"
#include "main.h"

RakNet__Send_t CSAMPFunctions::pfn__RakNet__Send = NULL;
RakNet__Receive_t CSAMPFunctions::pfn__RakNet__Receive = NULL;
RakNet__RPC_t CSAMPFunctions::pfn__RakNet__RPC = NULL;
RakNet__GetPlayerIDFromIndex_t CSAMPFunctions::pfn__RakNet__GetPlayerIDFromIndex = NULL;

void CSAMPFunctions::Initialize(void **pluginData)
{
	auto (*ptr)(void) = reinterpret_cast<void *(*)()>(pluginData[PLUGIN_DATA_NETGAME]);
	pNetGame = reinterpret_cast<CNetGame *>(ptr());

	// Get pConsole
	int (*pfn_GetConsole)(void) = (int (*)(void))pluginData[PLUGIN_DATA_CONSOLE];
	pConsole = (void *)pfn_GetConsole();

	// Get pRakServer
	int (*pfn_GetRakServer)(void) = (int (*)(void))pluginData[PLUGIN_DATA_RAKSERVER];
	pRakServer = (RakServer *)pfn_GetRakServer();

	int *pRakServer_VTBL = ((int *)(*(void **)pRakServer));

	POINT_TO_MEMBER(RakNet__Send, pRakServer_VTBL[RAKNET_SEND_OFFSET]);
	POINT_TO_MEMBER(RakNet__Receive, pRakServer_VTBL[RAKNET_RECEIVE_OFFSET]);
	POINT_TO_MEMBER(RakNet__RPC, pRakServer_VTBL[RAKNET_RPC_OFFSET]);
	POINT_TO_MEMBER(RakNet__GetPlayerIDFromIndex, pRakServer_VTBL[RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET]);

	Unlock((void *)&pRakServer_VTBL[RAKNET_RECEIVE_OFFSET], 4);
	pRakServer_VTBL[RAKNET_RECEIVE_OFFSET] = reinterpret_cast<int>(CHookRakServer::Receive);
}

void CSAMPFunctions::SpawnPlayer(int playerid)
{
	CPlayer__SpawnForWorld_t SpawnForWorld = (CPlayer__SpawnForWorld_t)CAddress::FUNC_CPlayer__SpawnForWorld;

	auto value = getNetGame([playerid](auto netGame, auto structs) -> void * {
		return netGame->pPlayerPool->pPlayer[playerid];
	});

	SpawnForWorld(value);
}

bool CSAMPFunctions::Send(RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast)
{
	return pfn__RakNet__Send(pRakServer, parameters, priority, reliability, orderingChannel, playerId, broadcast);
}

Packet *CSAMPFunctions::Receive(void *ppRakServer)
{
	return pfn__RakNet__Receive(ppRakServer);
}

bool CSAMPFunctions::RPC(int *uniqueID, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp)
{
	return pfn__RakNet__RPC(pRakServer, uniqueID, parameters, priority, reliability, orderingChannel, playerId, broadcast, shiftTimestamp);
}

PlayerID CSAMPFunctions::GetPlayerIDFromIndex(int index)
{
	return pfn__RakNet__GetPlayerIDFromIndex(pRakServer, index);
}