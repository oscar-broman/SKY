#ifndef SKY_NETWORK
#define SKY_NETWORK

#include "sdk.hpp"
#include "Shared/NetCode/vehicle.hpp"
#include "Shared/NetCode/core.hpp"

#include "main.hpp"
#include "player.hpp"

enum PacketEnumeration : int
{
    ID_TIMESTAMP = 40,
    ID_VEHICLE_SYNC = 200,
    ID_AIM_SYNC = 203,
    ID_BULLET_SYNC = 206,
    ID_PLAYER_SYNC = 207,
    ID_MARKERS_SYNC,
    ID_UNOCCUPIED_SYNC = 209,
    ID_TRAILER_SYNC = 210,
    ID_PASSENGER_SYNC = 211,
    ID_SPECTATOR_SYNC = 212,
    ID_RCON_COMMAND = 201,
    ID_RCON_RESPONCE,
    ID_WEAPONS_UPDATE = 204,
    ID_STATS_UPDATE = 205,
};

static bool IsPlayerUpdatePacket(int packetId)
{
    return (
        packetId == ID_PLAYER_SYNC ||
        packetId == ID_VEHICLE_SYNC ||
        packetId == ID_PASSENGER_SYNC ||
        packetId == ID_SPECTATOR_SYNC ||
        packetId == ID_AIM_SYNC ||
        packetId == ID_TRAILER_SYNC);
}

class PlayerSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

class AimSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

class VehicleSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

class PassengerSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

class SpectatorSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

class TrailerSync final : public SingleNetworkInEventHandler
{
    bool received(IPlayer &peer, NetworkBitStream &bs) override;
};

#endif