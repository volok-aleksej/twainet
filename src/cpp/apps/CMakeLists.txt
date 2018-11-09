cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(deamon)
add_subdirectory(remote_terminal)
