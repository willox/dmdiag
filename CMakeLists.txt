cmake_minimum_required(VERSION 3.16...3.17)

project(dmdiag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	add_definitions(-DNOMINMAX)
endif()

add_executable(dmdiag dm/internal_types.cpp dm/internal_types.h dmdiag.h dmdiag.cpp elf.h elf.cpp dm/State.h dm/State.cpp dm/VPtr.cpp dm/VPtr.h)

include_directories(dmdiag .)