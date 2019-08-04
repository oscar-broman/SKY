#include <unordered_map>
#include <plugincommon.h>
#include <amx/amx.h>

#include "Functions.h"
#include "Addresses.h"
#include "main.h"
#include "Hooks.h"


RakNet__Start_t								CSAMPFunctions::pfn__RakNet__Start = NULL;
RakNet__Send_t								CSAMPFunctions::pfn__RakNet__Send = NULL;
RakNet__Receive_t							CSAMPFunctions::pfn__RakNet__Receive = NULL;
RakNet__SetAllowedPlayers_t					CSAMPFunctions::pfn__RakNet__SetAllowedPlayers = NULL;
RakNet__GetLastPing_t						CSAMPFunctions::pfn__RakNet__GetLastPing = NULL;
RakNet__RegisterAsRemoteProcedureCall_t		CSAMPFunctions::pfn__RakNet__RegisterAsRemoteProcedureCall = NULL;
RakNet__UnregisterAsRemoteProcedureCall_t	CSAMPFunctions::pfn__RakNet__UnregisterAsRemoteProcedureCall = NULL;
RakNet__RPC_t								CSAMPFunctions::pfn__RakNet__RPC = NULL;
RakNet__GetLocalIP_t						CSAMPFunctions::pfn__RakNet__GetLocalIP = NULL;
RakNet__GetIndexFromPlayerID_t				CSAMPFunctions::pfn__RakNet__GetIndexFromPlayerID = NULL;
RakNet__GetPlayerIDFromIndex_t				CSAMPFunctions::pfn__RakNet__GetPlayerIDFromIndex = NULL;
RakNet__SetTimeoutTime_t					CSAMPFunctions::pfn__RakNet__SetTimeoutTime = NULL;
RakNet__AddToBanList_t						CSAMPFunctions::pfn__RakNet__AddToBanList = NULL;
RakNet__RemoveFromBanList_t					CSAMPFunctions::pfn__RakNet__RemoveFromBanList = NULL;
RakNet__ClearBanList_t						CSAMPFunctions::pfn__RakNet__ClearBanList = NULL;

void CSAMPFunctions::PreInitialize()
{
	/*INIT_FPTR(CConsole__AddStringVariable);
	INIT_FPTR(CConsole__GetStringVariable);
	INIT_FPTR(CConsole__SetStringVariable);
	INIT_FPTR(CConsole__GetIntVariable);
	INIT_FPTR(CConsole__SetIntVariable);
	INIT_FPTR(CConsole__GetBoolVariable);
	INIT_FPTR(CConsole__ModifyVariableFlags);
	INIT_FPTR(CConsole__FindVariable);
	INIT_FPTR(CConsole__SendRules);
	INIT_FPTR(CConsole__Execute);

	INIT_FPTR(CFilterscripts__LoadFilterscript);
	INIT_FPTR(CFilterscripts__UnLoadFilterscript);

	INIT_FPTR(CPlayer__SpawnForWorld);
	INIT_FPTR(CPlayerPool__HandleVehicleRespawn);
	INIT_FPTR(CObject__SpawnForPlayer);

	INIT_FPTR(Packet_WeaponsUpdate);
	INIT_FPTR(Packet_StatsUpdate);

	INIT_FPTR(format_amxstring);*/
}

void CSAMPFunctions::Initialize(void **pluginData)
{
	// Get pNetGame
	int(*pfn_GetNetGame)(void) = (int(*)(void))pluginData[PLUGIN_DATA_NETGAME];
	pNetGame = (CNetGame*)pfn_GetNetGame();

	// Get pConsole
	int(*pfn_GetConsole)(void) = (int(*)(void))pluginData[PLUGIN_DATA_CONSOLE];
	pConsole = (void*)pfn_GetConsole();

	// Get pRakServer
	int(*pfn_GetRakServer)(void) = (int(*)(void))pluginData[PLUGIN_DATA_RAKSERVER];
	pRakServer = (RakServer*)pfn_GetRakServer();

	int *pRakServer_VTBL = ((int*)(*(void**)pRakServer));
	//logprintf("pRakServer_VTBL: %x", pRakServer_VTBL);

	POINT_TO_MEMBER(RakNet__Start, pRakServer_VTBL[RAKNET_START_OFFSET]);
	POINT_TO_MEMBER(RakNet__Send, pRakServer_VTBL[RAKNET_SEND_OFFSET]);
	POINT_TO_MEMBER(RakNet__Receive, pRakServer_VTBL[RAKNET_RECEIVE_OFFSET]);
	POINT_TO_MEMBER(RakNet__SetAllowedPlayers, pRakServer_VTBL[RAKNET_SET_ALLOWED_PLAYERS_OFFSET]);
	POINT_TO_MEMBER(RakNet__GetLastPing, pRakServer_VTBL[RAKNET_GET_LAST_PING_OFFSET]);
	POINT_TO_MEMBER(RakNet__RegisterAsRemoteProcedureCall, pRakServer_VTBL[RAKNET_REGISTER_RPC_OFFSET]);
	POINT_TO_MEMBER(RakNet__UnregisterAsRemoteProcedureCall, pRakServer_VTBL[RAKNET_UNREGISTER_RPC_OFFSET]);
	POINT_TO_MEMBER(RakNet__RPC, pRakServer_VTBL[RAKNET_RPC_OFFSET]);
	POINT_TO_MEMBER(RakNet__GetLocalIP, pRakServer_VTBL[RAKNET_GET_LOCAL_IP_OFFSET]);
	POINT_TO_MEMBER(RakNet__GetIndexFromPlayerID, pRakServer_VTBL[RAKNET_GET_INDEX_FROM_PLAYERID_OFFSET]);
	POINT_TO_MEMBER(RakNet__GetPlayerIDFromIndex, pRakServer_VTBL[RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET]);
	POINT_TO_MEMBER(RakNet__SetTimeoutTime, pRakServer_VTBL[RAKNET_SET_TIMEOUT_OFFSET]);
	POINT_TO_MEMBER(RakNet__AddToBanList, pRakServer_VTBL[RAKNET_ADD_BAN_OFFSET]);
	POINT_TO_MEMBER(RakNet__RemoveFromBanList, pRakServer_VTBL[RAKNET_REMOVE_BAN_OFFSET]);
	POINT_TO_MEMBER(RakNet__ClearBanList, pRakServer_VTBL[RAKNET_CLEAR_BAN_OFFSET]);

	Unlock((void*)&pRakServer_VTBL[RAKNET_RECEIVE_OFFSET], 4); 
	pRakServer_VTBL[RAKNET_RECEIVE_OFFSET] = reinterpret_cast<int>(CHookRakServer::Receive);
}

void CSAMPFunctions::SpawnPlayer(int playerid)
{
	CPlayer__SpawnForWorld_t SpawnForWorld = (CPlayer__SpawnForWorld_t)CAddress::FUNC_CPlayer__SpawnForWorld;
	SpawnForWorld(pNetGame->pPlayerPool->pPlayer[playerid]);
}

bool CSAMPFunctions::Start(unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress)
{
	return pfn__RakNet__Start(pRakServer, AllowedPlayers, depreciated, threadSleepTimer, port, forceHostAddress);
}

bool CSAMPFunctions::Send(RakNet::BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast)
{
	return pfn__RakNet__Send(pRakServer, parameters, priority, reliability, orderingChannel, playerId, broadcast);
}

Packet* CSAMPFunctions::Receive(void* ppRakServer)
{
	return pfn__RakNet__Receive(ppRakServer);
}

void CSAMPFunctions::SetAllowedPlayers(unsigned short numberAllowed)
{
	pfn__RakNet__SetAllowedPlayers(pRakServer, numberAllowed);
}

int CSAMPFunctions::GetLastPing(const PlayerID playerId)
{
	return pfn__RakNet__GetLastPing(pRakServer, playerId);
}

void CSAMPFunctions::RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms))
{
	pfn__RakNet__RegisterAsRemoteProcedureCall(pRakServer, uniqueID, functionPointer);
}

void CSAMPFunctions::UnregisterAsRemoteProcedureCall(int* uniqueID)
{
	pfn__RakNet__UnregisterAsRemoteProcedureCall(pRakServer, uniqueID);
}

bool CSAMPFunctions::RPC(int* uniqueID, RakNet::BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp)
{
	return pfn__RakNet__RPC(pRakServer, uniqueID, parameters, priority, reliability, orderingChannel, playerId, broadcast, shiftTimestamp);
}

const char* CSAMPFunctions::GetLocalIP(unsigned int index)
{
	return pfn__RakNet__GetLocalIP(pRakServer, index);
}

int CSAMPFunctions::GetIndexFromPlayerID(PlayerID playerId)
{
	return pfn__RakNet__GetIndexFromPlayerID(pRakServer, playerId);
}

PlayerID CSAMPFunctions::GetPlayerIDFromIndex(int index)
{
	return pfn__RakNet__GetPlayerIDFromIndex(pRakServer, index);
}

void CSAMPFunctions::SetTimeoutTime(RakNetTime timeMS, const PlayerID target)
{
	pfn__RakNet__SetTimeoutTime(pRakServer, timeMS, target);
}

void CSAMPFunctions::AddToBanList(const char *IP, unsigned int milliseconds)
{
	pfn__RakNet__AddToBanList(pRakServer, IP, milliseconds);
}

void CSAMPFunctions::RemoveFromBanList(const char *IP)
{
	pfn__RakNet__RemoveFromBanList(pRakServer, IP);
}

void CSAMPFunctions::ClearBanList(void)
{
	pfn__RakNet__ClearBanList(pRakServer);
}