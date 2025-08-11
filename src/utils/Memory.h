#pragma once

#include "../structs/CTypes.h"
#include <cstddef>
#include <cstdint>

namespace Memory
{
    bool Unlock(void *address, size_t len);
    bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask);
    DWORD FindPattern(const char *pattern, const char *mask);
}