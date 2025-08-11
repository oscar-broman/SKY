#include "Functions.h"
#include "../core/Addresses.h"
#include "../core/Hooks.h"
#include "../utils/Memory.h"
#include "../structs/Versions.h"
#include "../core/main.h"
#include <raknet/NetworkTypes.h>
#include <cstdio>

RakNet__Send_t CSAMPFunctions::pfn__RakNet__Send = 0;
RakNet__Receive_t CSAMPFunctions::pfn__RakNet__Receive = 0;
RakNet__RPC_t CSAMPFunctions::pfn__RakNet__RPC = 0;
RakNet__GetPlayerIDFromIndex_t CSAMPFunctions::pfn__RakNet__GetPlayerIDFromIndex = 0;
RakNet__GetIndexFromPlayerID_t CSAMPFunctions::pfn__RakNet__GetIndexFromPlayerID = 0;
RakNet__AddToBanList_t CSAMPFunctions::pfn__RakNet__AddToBanList = 0;
RakNet__RemoveFromBanList_t CSAMPFunctions::pfn__RakNet__RemoveFromBanList = 0;
RakNet__SetTimeoutTime_t CSAMPFunctions::pfn__RakNet__SetTimeoutTime = 0;

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
	POINT_TO_MEMBER(RakNet__GetIndexFromPlayerID, pRakServer_VTBL[RAKNET_GET_INDEX_FROM_PLAYERID_OFFSET]);
	POINT_TO_MEMBER(RakNet__AddToBanList, pRakServer_VTBL[RAKNET_ADD_BAN_OFFSET]);
	POINT_TO_MEMBER(RakNet__RemoveFromBanList, pRakServer_VTBL[RAKNET_REMOVE_BAN_OFFSET]);
	POINT_TO_MEMBER(RakNet__SetTimeoutTime, pRakServer_VTBL[RAKNET_SET_TIMEOUT_OFFSET]);

	Memory::Unlock((void *)&pRakServer_VTBL[RAKNET_RECEIVE_OFFSET], 4);
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

int CSAMPFunctions::GetIndexFromPlayerID(PlayerID playerId)
{
	return pfn__RakNet__GetIndexFromPlayerID(pRakServer, playerId);
}

void CSAMPFunctions::AddToBanList(const char *IP, unsigned int milliseconds)
{
	pfn__RakNet__AddToBanList(pRakServer, IP, milliseconds);
}

void CSAMPFunctions::RemoveFromBanList(const char *IP)
{
	pfn__RakNet__RemoveFromBanList(pRakServer, IP);
}

void CSAMPFunctions::SetTimeoutTime(unsigned int timeMS, const PlayerID target)
{
	pfn__RakNet__SetTimeoutTime(pRakServer, timeMS, target);
}

void CSAMPFunctions::TimeoutPlayer(int playerId, unsigned int timeoutMs)
{
	PlayerID target = GetPlayerIDFromIndex(playerId);
	if (target.binaryAddress == 0xFFFFFFFF && target.port == 0xFFFF) {
		// Invalid player ID
		return;
	}

	unsigned int ip = target.binaryAddress;
	char playerIP[16];
	sprintf(playerIP, "%u.%u.%u.%u",
		ip & 0xFF,
		(ip >> 8) & 0xFF,
		(ip >> 16) & 0xFF,
		(ip >> 24) & 0xFF);

	AddToBanList(playerIP, timeoutMs);

	printf("[SKY] Player %d (IP: %s) timed out for %u ms\n", playerId, playerIP, timeoutMs);
}