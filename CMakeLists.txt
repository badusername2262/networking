cmake_minimum_required(VERSION 3.16)
project(networking_cli)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

add_executable(networking_cli networking.cpp)

target_link_libraries(networking_cli OpenSSL::SSL OpenSSL::Crypto)