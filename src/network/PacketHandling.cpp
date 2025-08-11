#include "PacketHandling.h"
#include "../api/Functions.h"
#include "../game/Player.h"
#include "RPCs.h"
#include "net.h"
#include <cstdio>
#include <algorithm>
#include <raknet/NetworkTypes.h>

namespace PacketHandling
{
    static int invalidPacketCount[MAX_PLAYERS] = {0};

    int maxInvalidPacketsThreshold = DEFAULT_MAX_INVALID_PACKETS_THRESHOLD;
    DWORD timeoutDurationMs = DEFAULT_TIMEOUT_DURATION_MS;

    void TimeoutPlayerForSuspiciousActivity(WORD playerId, const char* reason)
    {
        CSAMPFunctions::TimeoutPlayer(playerId, timeoutDurationMs);
        printf("[TIMEOUT] Player %d has been timed out for %dms. Reason: %s\n", 
               playerId, timeoutDurationMs, reason);
    }

    void HandleInvalidPacket(WORD playerId, const char* packetType)
    {
        invalidPacketCount[playerId]++;

        if (invalidPacketCount[playerId] >= maxInvalidPacketsThreshold) {
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

    void SetMaxInvalidPacketsThreshold(int threshold)
    {
        maxInvalidPacketsThreshold = std::clamp(threshold, 0, MAX_INVALID_PACKETS_THRESHOLD);
    }

    int GetMaxInvalidPacketsThreshold()
    {
        return maxInvalidPacketsThreshold;
    }

    void SetTimeoutDuration(DWORD durationMs)
    {
        timeoutDurationMs = std::clamp(durationMs, static_cast<DWORD>(0), MAX_TIMEOUT_DURATION_MS);
    }

    DWORD GetTimeoutDuration()
    {
        return timeoutDurationMs;
    }
}
