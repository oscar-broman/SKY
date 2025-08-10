#include "PacketHandling.h"
#include "Functions.h"
#include "Player.h"
#include "RPCs.h"
#include "net.h"
#include <cstdio>
#include <raknet/NetworkTypes.h>

namespace PacketHandling
{
    static int invalidPacketCount[MAX_PLAYERS] = {0};

    void TimeoutPlayerForSuspiciousActivity(WORD playerId, const char* reason)
    {
        CSAMPFunctions::TimeoutPlayer(playerId, TIMEOUT_DURATION_MS);
        printf("[TIMEOUT] Player %d has been timed out for %dms. Reason: %s\n", 
               playerId, TIMEOUT_DURATION_MS, reason);
    }

    void HandleInvalidPacket(WORD playerId, const char* packetType)
    {
        invalidPacketCount[playerId]++;

        if (invalidPacketCount[playerId] >= MAX_INVALID_PACKETS_THRESHOLD) {
            char reason[256];
            sprintf(reason, "Too many invalid %s packets (%d)", packetType, invalidPacketCount[playerId]);
            TimeoutPlayerForSuspiciousActivity(playerId, reason);
            invalidPacketCount[playerId] = 0;
        }
    }

    void ResetPlayerInvalidPacketCount(WORD playerId)
    {
        if (playerId >= 0 && playerId < MAX_PLAYERS) {
            invalidPacketCount[playerId] = 0;
        }
    }

    bool IsPlayerUpdatePacket(unsigned char packetId)
    {
        return (packetId == ID_PLAYER_SYNC ||
                packetId == ID_VEHICLE_SYNC ||
                packetId == ID_PASSENGER_SYNC ||
                packetId == ID_SPECTATOR_SYNC ||
                packetId == ID_AIM_SYNC ||
                packetId == ID_TRAILER_SYNC);
    }

    BYTE GetPacketID(Packet *packet)
    {
        if (packet == nullptr) {
            return 255;
        }

        if ((unsigned char)packet->data[0] == ID_TIMESTAMP) {
            if (packet->length <= sizeof(unsigned char) + sizeof(unsigned long)) {
                return 255;
            }
            return (unsigned char)packet->data[sizeof(unsigned char) + sizeof(unsigned long)];
        }

        return (unsigned char)packet->data[0];
    }
}
