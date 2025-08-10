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
 *  the Initial Developer. All Rights Reserved.
 *
 *  Contributor(s):
 *
 *  Peter Beverloo
 *  Marcus Bauer
 *  MaVe;
 *  Sammy91
 *  Incognito
 *
 *  Special Thanks to:
 *
 *  SA:MP Team past, present and future
 */

#include "Hooks.h"

#include "Addresses.h"
#include "Functions.h"
#include "Player.h"
#include "RPCs.h"
#include "Scripting.h"
#include "Utils.h"
#include "Versions.h"
#include "main.h"

#include "Animation.h"
#include "Memory.h"
#include "PacketHandling.h"
#include "SyncProcessing.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <cstdarg>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#endif

using std::isfinite;
extern void *pAMXFunctions;

Packet *THISCALL CHookRakServer::Receive(void *ppRakServer)
{
	return Versions::getNetGame([ppRakServer](auto netGame, auto structs) -> Packet *
								{
        using Structs = decltype(structs);

        Packet *packet = CSAMPFunctions::Receive(ppRakServer);

        BYTE packetId = PacketHandling::GetPacketID(packet);
        if (packetId == 0xFF) {
            return packet;
        }

        WORD playerId = packet->playerIndex;
        if (!ValidatePlayerID(playerId)) {
            return nullptr;
        }

        if (!Player::CheckPacketRateLimit(playerId)) {
            // Instead of just dropping packets, timeout the player for excessive packet spam
            PacketHandling::TimeoutPlayerForSuspiciousActivity(playerId, "Packet rate limit exceeded");
            return nullptr;
        }

        Player::lastUpdateTick[playerId] = GetTickCount();

        switch (packetId) {
            case ID_PLAYER_SYNC:
                if (!SyncProcessing::ProcessPlayerSync<Structs>(packet, playerId)) {
                    PacketHandling::HandleInvalidPacket(playerId, "PLAYER_SYNC");
                    return nullptr;
                }
                break;

            case ID_AIM_SYNC:
                if (!SyncProcessing::ProcessAimSync<Structs>(packet, playerId)) {
                    PacketHandling::HandleInvalidPacket(playerId, "AIM_SYNC");
                    return nullptr;
                }
                break;

            case ID_VEHICLE_SYNC:
                if (!SyncProcessing::ProcessVehicleSync<Structs>(packet, playerId)) {
                    PacketHandling::HandleInvalidPacket(playerId, "VEHICLE_SYNC");
                    return nullptr;
                }
                break;

            case ID_PASSENGER_SYNC:
                if (!SyncProcessing::ProcessPassengerSync<Structs>(packet, playerId)) {
					PacketHandling::HandleInvalidPacket(playerId, "PASSENGER_SYNC");
                    return nullptr;
                }
                break;

            case ID_SPECTATOR_SYNC:
                if (!SyncProcessing::ProcessSpectatorSync<Structs>(packet, playerId)) {
                    PacketHandling::HandleInvalidPacket(playerId, "SPECTATOR_SYNC");
                    return nullptr;
                }
                break;

            default:
                // For other packet types, just pass through
                break;
        }

        return packet; });
}

void InstallPreHooks()
{
	std::memset(&Player::fakeHealth, 255, sizeof(Player::fakeHealth));
	std::memset(&Player::fakeArmour, 255, sizeof(Player::fakeArmour));
	std::memset(&Player::fakeQuat[0], NULL, sizeof(Player::fakeQuat));

	for (int playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		Player::InitializeRateLimits(playerId);
		PacketHandling::ResetPlayerInvalidPacketCount(playerId);
	}
}