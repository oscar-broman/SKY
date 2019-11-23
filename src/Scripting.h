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

#include "main.h"

#define CHECK_PARAMS(m, n)                                                                             \
	if (params[0] != (m * 4))                                                                          \
	{                                                                                                  \
		logprintf("SKY: %s: Expecting %d parameter(s), but found %d", n, m, params[0] / sizeof(cell)); \
		return 0;                                                                                      \
	}

int InitScripting(AMX *amx);
#endif
