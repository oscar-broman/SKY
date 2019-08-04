#ifndef AMXFUNCTIONS_H
#define AMXFUNCTIONS_H

#include <plugincommon.h>
#include <amx/amx.h>

//void Redirect(AMX * amx, char const * const from, ucell to, AMX_NATIVE * store);
int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max);

#endif
