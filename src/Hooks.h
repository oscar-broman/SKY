#ifndef HOOKS_H
#define HOOKS_H

#include "CTypes.h"
#include "main.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <raknet/NetworkTypes.h>

// Hopefully this will be optimised out
// Unfortunately I can't get ANY direct cast to work
// pointer_to_member seems like a very special structure
// Edit: Seems like it is optimised out, good, so this
// just bypasses the compilers checks to do what I want
#define POINTER_TO_MEMBER(m, n, o) temp = n, m = *(o *)&temp

// Definitions
bool Unlock(void *address, size_t len);

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask);
DWORD FindPattern(const char *pattern, const char *mask);

void InstallPreHooks();

BYTE GetPacketID(Packet *p);

#endif
