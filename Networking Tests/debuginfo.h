#pragma once

#ifdef MY_DEBUG_ALLOC_TRACKING

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

void PrintMemoryUsage()
{
    std::cout << "Memory Usage: " << mem.CurrentUsage() << " bytes\n";
}

#else

inline void PrintMemoryUsage() {}

#endif