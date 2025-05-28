#pragma once

// windows only includes
#include <WinSock2.h>
#include <WS2tcpip.h>

void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <client|server>\n", program_name);
}

size_t HashPassword(std::string input)
{   
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    std::hash<std::string> str_hash;
    std::hash<long long> time_hash;

    size_t hash1 = str_hash(input);
    size_t hash2 = time_hash(nanos);

    return hash1 ^ (hash2 << 1);
}