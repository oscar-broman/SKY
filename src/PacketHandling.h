#pragma once

#include "CTypes.h"
#include "net.h"

// Forward declaration
struct Packet;

namespace PacketHandling
{
    constexpr int MAX_INVALID_PACKETS_THRESHOLD = 5;
    constexpr DWORD TIMEOUT_DURATION_MS = 60000; // 60 seconds

    void TimeoutPlayerForSuspiciousActivity(WORD playerId, const char* reason);
    void HandleInvalidPacket(WORD playerId, const char* packetType);
    void ResetPlayerInvalidPacketCount(WORD playerId);
    
    bool IsPlayerUpdatePacket(unsigned char packetId);
    BYTE GetPacketID(Packet *packet);
}
