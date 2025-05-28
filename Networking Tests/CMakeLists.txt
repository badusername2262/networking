cmake_minimum_required(VERSION 3.16)
project(networking_cli)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)

set(SRC 
        networking.cpp)

set(HEADERS
        networking.h
        debuginfo.h)

add_executable(networking_cli ${SRC} ${HEADERS})

target_compile_definitions(networking_cli PRIVATE $<$<CONFIG:Debug>:MY_DEBUG_ALLOC_TRACKING>)

target_precompile_headers(networking_cli PRIVATE pch.h)

target_link_libraries(networking_cli Ws2_32)