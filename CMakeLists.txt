cmake_minimum_required(VERSION 3.29)
project(opcua_server C)

set(CMAKE_C_STANDARD 11)

add_library(opcua_server
        opcua_server.c)
target_link_libraries(opcua_server yyjson)