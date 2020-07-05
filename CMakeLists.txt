cmake_minimum_required(VERSION 3.16...3.17)

project(dmdiag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dmdiag internal_types.h dmdiag.h dmdiag.cpp elf.h)
