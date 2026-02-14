cmake_minimum_required (VERSION 3.10)

project (basalt)

set (CMAKE_CXX_STANDARD 20)

add_executable (basalt
	src/main.cpp
 "src/tty.h"  "src/defs.h" "src/builder.h" "src/builder.cpp")

if(MSVC)
    target_compile_options(basalt PUBLIC "/Zc:__cplusplus")
endif()