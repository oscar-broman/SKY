#include "RPCs.h"

#include "main.h"

#include "Functions.h"
#include "Utils.h"
#include "Addresses.h"

int RPC_SelectObject = 0x1B; // 27
int RPC_Spawn = 0x34; // 52
int RPC_Death = 0x35; // 53
int RPC_Chat = 0x65; // 101
int RPC_ClientCheck = 0x67; // 103
							//int RPC_EditObject			= 0x75; // 117
int RPC_PickedUpPickup = 0x83; // 131
int RPC_UpdateScoresPingsIPs = 0x9B; // 155

int RPC_SetPlayerName = 0x0B; // 11
int RPC_WorldPlayerAdd = 0x20; // 32
int RPC_CreateObject = 0x2C; // 44
int RPC_SetObjectPos = 0x2D; // 45
int RPC_SetObjectRot = 0x2E; // 46
int RPC_DestroyObject = 0x2F; // 47
int RPC_ChatBubble = 0x3B; // 59
int RPC_DestroyPickup = 0x3F; // 63
int RPC_SetPlayerTeam = 0x45; // 69
int RPC_SetPlayerColor = 0x48; // 72
int RPC_DisplayGameText = 0x49; // 73
int RPC_AttachObject = 0x4B; // 75
int RPC_AttachCameraToObject = 0x51; // 81
int RPC_SetObjectMaterial = 0x54; // 84
int RPC_StopFlashGangZone = 0x55; // 85
int RPC_ApplyAnimation = 0x56; // 86
int RPC_SetFightingStyle = 0x59; // 89
int RPC_ClientMessage = 0x5D; // 93
int RPC_CreatePickup = 0x5F; // 95
int RPC_ScmEvent = 0x60; // 96
int RPC_MoveObject = 0x63; // 99
int RPC_SetTextDrawString = 0x69; // 105
int RPC_ShowGangZone = 0x6C; // 108
int RPC_Widescreen = 0x6F; // 111
int RPC_SetPlayerAttachedObject = 0x71; // 113
int RPC_EditObject = 0x75; // 117
int RPC_HideGangZone = 0x78; // 120
int RPC_FlashGangZone = 0x79; // 121
int RPC_StopObject = 0x74; // 122
int RPC_ServerJoin = 0x89; // 137
int RPC_ServerQuit = 0x8A; // 138
int RPC_InitGame = 0x8B; // 139
int RPC_SetGravity = 0x92; // 146
int RPC_SetPlayerSkin = 0x99; // 153
int RPC_WorldPlayerRemove = 0xA3; // 163
int RPC_DeathBroadcast = 0xA6; // 166

int RPC_SendArtList = 0xB3; // 179

int RPC_ClearAnimations = 0x57;
int RPC_ScrApplyAnimation = 0x56;
int RPC_ScrEditTextDraw = 0x69;
