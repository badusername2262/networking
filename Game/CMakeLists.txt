cmake_minimum_required(VERSION 3.16)
project(Game)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(include/ include/Extern/)

add_subdirectory(Include/Extern/glfw/)

set(GLAD_SRC Include/Extern/glad/src/gl.c)

set(SRC main.cpp)

set(HEADERS )

add_executable(Game ${SRC} ${HEADERS} ${GLAD_SRC})

target_compile_definitions(Game PRIVATE _AMD64_)

if(WIN32)
target_link_libraries(Game glfw opengl32)
elseif(LINUX)
target_link_libraries(Game glfw)
endif()