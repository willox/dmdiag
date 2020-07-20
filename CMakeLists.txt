cmake_minimum_required(VERSION 3.16...3.17)

project(dmdiag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	add_definitions(-DNOMINMAX)
endif()




include(ExternalProject)
ExternalProject_Add( 
	squirrel
	GIT_REPOSITORY https://github.com/albertodemichelis/squirrel
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/squirrel
)
add_library(squirrel_static STATIC IMPORTED)
set_target_properties(squirrel_static PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/squirrel/lib/squirrel_static.lib)
target_include_directories(squirrel_static INTERFACE ${CMAKE_BINARY_DIR}/squirrel/include)
add_library(sqstdlib_static STATIC IMPORTED)
set_target_properties(sqstdlib_static PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/squirrel/lib/sqstdlib_static.lib)
target_include_directories(sqstdlib_static INTERFACE ${CMAKE_BINARY_DIR}/squirrel/include)



add_executable(dmdiag dm/internal_types.cpp dm/internal_types.h dmdiag.h dmdiag.cpp elf.h elf.cpp dm/dm.h dm/State.h dm/State.cpp dm/VPtr.cpp dm/VPtr.h dm/Mob.h dm/Mob.cpp)
target_link_libraries(dmdiag squirrel_static sqstdlib_static)

include_directories(dmdiag .)
