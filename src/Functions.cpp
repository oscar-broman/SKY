#include "Functions.h"
#include "Addresses.h"
#include "Hooks.h"
#include "Versions.h"
#include "main.h"
#include <raknet/NetworkTypes.h>

RakNet__Send_t CSAMPFunctions::pfn__RakNet__Send = 0;
RakNet__Receive_t CSAMPFunctions::pfn__RakNet__Receive = 0;
RakNet__RPC_t CSAMPFunctions::pfn__RakNet__RPC = 0;
RakNet__GetPlayerIDFromIndex_t CSAMPFunctions::pfn__RakNet__GetPlayerIDFromIndex = 0;

void CSAMPFunctions::Initialize()
{
	auto (*ptr)(void) = reinterpret_cast<void *(*)()>(ppPluginData[PLUGIN_DATA_NETGAME]);
	pNetGame = reinterpret_cast<CNetGame *>(ptr());

	// Get pConsole
	int (*pfn_GetConsole)(void) = (int (*)(void))ppPluginData[PLUGIN_DATA_CONSOLE];
	pConsole = (void *)pfn_GetConsole();

	// Get pRakServer
	int (*pfn_GetRakServer)(void) = (int (*)(void))ppPluginData[PLUGIN_DATA_RAKSERVER];
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
	Versions::getNetGame([playerid](auto netGame, auto structs) {
		CPlayer__SpawnForWorld_t SpawnForWorld = (CPlayer__SpawnForWorld_t)CAddress::FUNC_CPlayer__SpawnForWorld;
		SpawnForWorld(netGame->pPlayerPool->pPlayer[playerid]);
	});
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