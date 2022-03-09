cmake_minimum_required(VERSION 3.2)

project(lua-support)

set (CMAKE_CXX_STANDARD 11)

set(LUA_PATH "${PROJECT_SOURCE_DIR}/lua")
message(STATUS "lua path: ${LUA_PATH}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_subdirectory(lua-cmake)
add_subdirectory(lslua)
add_subdirectory(sample)
