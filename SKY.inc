#if defined _INC_SKY
    #endinput
#endif
#define _INC_SKY

// Make a player appear to spawn for all players but himself
native SpawnPlayerForWorld(playerid);
// Keep re-sending the last received sync data
native FreezeSyncData(playerid, bool:toggle);
// Set the HP bar (warning: affects GetPlayerHealth)
native SetFakeHealth(playerid, health);
// Set the armour bar (warning: affects GetPlayerArmour)
native SetFakeArmour(playerid, armour);
// Force a specific facing angle to sync for other players
native SetFakeFacingAngle(playerid, Float:angle = Float:0x7FFFFFFF);
// Disable stealth-knife sync (only the player doing it will see it happen)
native SetKnifeSync(toggle);
// Make a player appear dead for other players
native SendDeath(playerid);
// Set the last animation data
native SetLastAnimationData(playerid, data);
// Send the last sync data
// If an animation is specified, it will be similar to ApplyAnimation
native SendLastSyncData(playerid, toplayerid, animation = 0);
// Clear animations for another player only
native ClearAnimationsForPlayer(playerid, forplayerid);
// Disable infinity ammo bugs and other bugs (some bugs still work)
native SetDisableSyncBugs(toggle);
// Make the weapon state always synced as WEAPONSTATE_MORE_BULLETS
native SetInfiniteAmmoSync(playerid, toggle);
// Stop syncing keys for a player
native SetKeySyncBlocked(playerid, toggle);
// Same as YSF (renamed to avoid problems)
native TextDrawSetPosition(Text:text, Float:x, Float:y);
// Same as YSF (renamed to avoid problems)
native PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:x, Float:y);
// Set the string of a TextDraw per-player
native TextDrawSetStrForPlayer(Text:text, playerid, const string[]);
