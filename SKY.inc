#if defined _INC_SKY
    #endinput
#endif
#define _INC_SKY

// Sync Packets (kept for compatibility but only used for freeze functions)
enum E_SYNC_TYPES
{
    E_PLAYER_SYNC = 0,
    E_AIM_SYNC,
    E_VEHICLE_SYNC,
    E_PASSENGER_SYNC,
    E_SPECTATING_SYNC,
    E_LAST_SYNC,
    E_ALL_SYNC
};

// Set the maximum number of invalid packets before a player is timed out
native SetMaxInvalidPacketsThreshold(threshold);
// Get the current maximum number of invalid packets threshold
native GetMaxInvalidPacketsThreshold();
// Set the duration for which a player is timed out after exceeding invalid packets threshold
native SetInvalidPacketTimeout(durationMs);
// Get the current timeout duration for invalid packets
native GetInvalidPacketTimeout();

// Set packet rate limit for all packets (applies to all players)
native SetPacketRateLimit(maxPackets, timeWindowMs);
// Get current packet rate limit settings
native GetPacketRateLimit(&maxPackets, &timeWindowMs);
// Reset rate limits to defaults for all players
native ResetPacketRateLimits();

// Keep re-sending the last recieved sync packet of a certain type
native FreezeSyncPacket(playerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, bool:toggle);
// Send the last sync packet
// If a type is specified it will sync that packet only
// If an animation is specified, it will be similar to ApplyAnimation
native SendLastSyncPacket(playerid, toplayerid, E_SYNC_TYPES:type = E_PLAYER_SYNC, animation = 0);

// Disable infinity ammo bugs and other bugs (some bugs still work)
native SetDisableSyncBugs(toggle);
// Make the weapon state always synced as WEAPONSTATE_MORE_BULLETS
native SetInfiniteAmmoSync(playerid, toggle);
// Stop syncing keys for a player
native SetKeySyncBlocked(playerid, toggle);
// Disable stealth-knife sync (only the player doing it will see it happen)
native SetKnifeSync(toggle);

// Set the HP bar (warning: affects GetPlayerHealth)
native SetFakeHealth(playerid, health);
// Set the armour bar (warning: affects GetPlayerArmour)
native SetFakeArmour(playerid, armour);
// Force a specific facing angle to sync for other players
native SetFakeFacingAngle(playerid, Float:angle = Float:0x7FFFFFFF);

// Make a player appear to spawn for all players but himself
native SpawnPlayerForWorld(playerid);
// Make a player appear dead for other players
native SendDeath(playerid);

// Set the last animation data
native SetLastAnimationData(playerid, data);
// Clear animations for another player only
native ClearAnimationsForPlayer(playerid, forplayerid);

// Same as YSF (renamed to avoid problems)
native TextDrawSetPosition(Text:text, Float:x, Float:y);
// Same as YSF (renamed to avoid problems)
native PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:x, Float:y);
// Set the string of a TextDraw per-player
native TextDrawSetStrForPlayer(Text:text, playerid, const string[]);

// Keep re-sending the last received sync data
#pragma deprecated Please use FreezeSyncPacket instead
native FreezeSyncData(playerid, bool:toggle);
// Send the last sync data
// If an animation is specified, it will be similar to ApplyAnimation
#pragma deprecated Please use SendLastSyncPacket instead
native SendLastSyncData(playerid, toplayerid, animation = 0);
