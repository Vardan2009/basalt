cmake_minimum_required (VERSION 3.10)

project (basalt)

set (CMAKE_CXX_STANDARD 20)

set (CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_subdirectory (src/vendor/yaml-cpp)
add_subdirectory (src/vendor/maddy)

set (MyHTML_BUILD_WITHOUT_THREADS "Build without POSIX Threads" ON)
set (MyCORE_BUILD_WITHOUT_THREADS "Build MyCORE without POSIX Threads" ON)
set (MyHTML_BUILD_SHARED "Build shared library" OFF)
set (MyHTML_BUILD_STATIC "Build static library" ON)
add_subdirectory (src/vendor/myhtml)

add_executable (basalt
	src/main.cpp
	src/tty.h
	src/defs.h
	src/builder.h
	src/builder.cpp
)


target_link_libraries (basalt PUBLIC yaml-cpp::yaml-cpp maddy myhtml_static)
target_include_directories (basalt PUBLIC src/vendor/myhtml/include) # this sucks, but whatever

if (MSVC)
    target_compile_options (basalt PUBLIC "/Zc:__cplusplus")
endif ()