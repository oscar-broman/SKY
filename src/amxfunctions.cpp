#include <amx/amx.h>

int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max)
{
	cell* dest = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	cell* start = dest;
	while (max--&&*source)
		*dest++ = (cell)*source++;
	*dest = 0;
	return dest - start;
}