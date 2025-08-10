#pragma once

#include "CTypes.h"
#include "net.h"

// Forward declaration
struct Packet;

namespace PacketHandling
{
    constexpr int DEFAULT_MAX_INVALID_PACKETS_THRESHOLD = 5;
    constexpr DWORD DEFAULT_TIMEOUT_DURATION_MS = 60000; // 60 seconds
    constexpr DWORD MAX_TIMEOUT_DURATION_MS = 3600000; // 60 minutes
    constexpr int MAX_INVALID_PACKETS_THRESHOLD = 100;

    extern int maxInvalidPacketsThreshold;
    extern DWORD timeoutDurationMs;

    void TimeoutPlayerForSuspiciousActivity(WORD playerId, const char* reason);
    void HandleInvalidPacket(WORD playerId, const char* packetType);
    void ResetPlayerInvalidPacketCount(WORD playerId);
    
    bool IsPlayerUpdatePacket(unsigned char packetId);
    BYTE GetPacketID(Packet *packet);

    // Configuration functions
    void SetMaxInvalidPacketsThreshold(int threshold);
    int GetMaxInvalidPacketsThreshold();
    void SetTimeoutDuration(DWORD durationMs);
    DWORD GetTimeoutDuration();
}
