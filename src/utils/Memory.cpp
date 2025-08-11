#include "Memory.h"

#include <cstddef>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifndef PAGESIZE
#define PAGESIZE (4096)
#endif

namespace Memory
{
    bool Unlock(void *address, size_t len)
    {
#ifdef _WIN32
        DWORD oldProtection;
        return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldProtection);
#else
        size_t pageSize = getpagesize();
        size_t alignedAddr = ((reinterpret_cast<uint32_t>(address) / pageSize) * pageSize);
        return !mprotect(reinterpret_cast<void *>(alignedAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
    }

    bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
    {
        for (; *mask; ++mask, ++data, ++pattern) {
            if (*mask == 'x' && *data != *pattern) {
                return false;
            }
        }
        return (*mask) == 0;
    }

    DWORD FindPattern(const char *pattern, const char *mask)
    {
        DWORD address;
        DWORD size;

#ifdef _WIN32
        MODULEINFO info = {0};
        address = (DWORD)GetModuleHandle(NULL);
        GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
        size = (DWORD)info.SizeOfImage;
#else
        address = 0x804b480; // around the elf base
        size = 0x8128B80 - address;
#endif

        for (DWORD i = 0; i < size; ++i) {
            if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask)) {
                return (DWORD)(address + i);
            }
        }
        return 0;
    }
}