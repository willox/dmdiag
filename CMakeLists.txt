cmake_minimum_required(VERSION 3.16...3.17)

project(dmdiag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	add_definitions(-DNOMINMAX)
endif()

add_library(dm dm/elf.h dm/elf.cpp dm/internal_types.cpp dm/internal_types.h dm/dm.h dm/State.h dm/State.cpp dm/VPtr.cpp dm/VPtr.h dm/Mob.h dm/Mob.cpp dm/String.h dm/String.cpp dm/Value.h dm/Value.cpp dm/Proc.h dm/Proc.cpp)
add_executable(dmdiag dmdiag.h dmdiag.cpp)
target_link_libraries(dmdiag dm)
