#include "RPCs.h"

#include "main.h"

#include "Structs.h"
#include "Functions.h"
#include "Utils.h"

#include <sdk/plugin.h>

int RPC_Gravity = 0x92;
int RPC_Weather = 0x98;
int RPC_Explosion = 0x4F;
int RPC_CreatePickup = 95;
int RPC_DestroyPickup = 63;
int RPC_SetPlayerTeam = 45;
int RPC_CreateObject = 0x2C;
int RPC_DestroyObject = 0x2F;
int RPC_AttachObject = 0x4B;
int RPC_Widescreen = 111;
int RPC_WorldBounds = 0x11;
int RPC_ShowGangZone = 0x6C;
int RPC_HideGangZone = 0x78;
int RPC_FlashGangZone = 0x79;
int RPC_StopFlashGangZone = 0x55;
int RPC_RemovePlayerAttachedObject = 0x71;
int RPC_WorldPlayerAdd = 32;
int RPC_WorldPlayerRemove = 163;
int RPC_ChatBubble = 0x3B;
int RPC_ScrApplyAnimation = 0x56;

int RPC_UpdateScoresPingsIPs = 0x9B;
int RPC_PickedUpPickup = 0x83;
int RPC_Spawn = 0x81;
int RPC_Death = 0x35;
int RPC_DeathBroadcast = 0xA6;

int RPC_ClearAnimations = 0x57;
