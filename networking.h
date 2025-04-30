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

#include <iomanip>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

int verify_callback(int preverify_ok, X509_STORE_CTX* ctx) {
    return preverify_ok; // return 0 to reject, 1 to ignore errors
}

void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <client|server>\n", program_name);
}