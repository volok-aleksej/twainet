cmake_minimum_required(VERSION 2.6)
project(twainet_libs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(thread_lib)
add_subdirectory(connector_lib)
add_subdirectory(ipc_lib)
add_subdirectory(client_server_lib)
add_subdirectory(tunnel_lib)