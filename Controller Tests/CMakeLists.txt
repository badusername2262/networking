cmake_minimum_required(VERSION 3.16)
project(Controller_test)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)

set(SRC 
        main.cpp)

set(HEADERS
        )

add_executable(Controller_test ${SRC} ${HEADERS})

target_compile_definitions(Controller_test PRIVATE _AMD64_)

target_precompile_headers(Controller_test PRIVATE )

if(WIN32)
target_link_libraries(Controller_test Xinput Dinput8 dxguid)
elseif(LINUX)
target_link_libraries(Controller_test )
endif()