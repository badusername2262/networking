#pragma once

// windows only includes
#include <WinSock2.h>
#include <WS2tcpip.h>

// input/output and data types
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <map>

void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <client|server>\n", program_name);
}