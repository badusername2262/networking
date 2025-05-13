#pragma once

#include <iostream>

struct Memory {
    uint32_t Allocated = 0;
    uint32_t Freed = 0;

    uint32_t CurrentUsage() { return Allocated - Freed; }
};

Memory mem;

void* operator new(size_t size)
{
    mem.Allocated += size;
    
    return malloc(size);
}

void operator delete(void* memory, size_t size)
{
    mem.Freed += size;

    free(memory);
}

static void PrintMemoryUsage()
{
    std::cout << "Memory Usage: " << mem.CurrentUsage() << " bytes\n";
}