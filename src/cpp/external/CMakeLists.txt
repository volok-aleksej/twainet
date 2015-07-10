cmake_minimum_required(VERSION 2.6)
project(twainet_libs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(protobuf-2.5.0)
add_subdirectory(udt4)