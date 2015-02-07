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

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <sdk/plugin.h>
#include "main.h"

#define CHECK_PARAMS(m, n) \
	if (params[0] != (m * 4)) \
	{ \
		logprintf("YSF: %s: Expecting %d parameter(s), but found %d", n, m, params[0] / sizeof(cell)); \
		return 0; \
	}

namespace Natives
{
static cell AMX_NATIVE_CALL SpawnPlayerForWorld(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetFakeHealth(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetFakeArmour(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetFakeFacingAngle(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL FreezeSyncData(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetKnifeSync(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SendDeath(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetLastAnimationData(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SendLastSyncData(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetDisableSyncBugs(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL ClearAnimationsForPlayer(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetKeySyncBlocked(AMX *amx, cell *params);
static cell AMX_NATIVE_CALL SetInfiniteAmmoSync(AMX *amx, cell *params);
};

int InitScripting(AMX *amx);

extern AMX_NATIVE_INFO RedirecedtNatives[];
#endif
