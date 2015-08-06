cmake_minimum_required(VERSION 2.6)
project(twainet)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(lib)
add_subdirectory(external/udt4)
add_subdirectory(twainet)
add_subdirectory(test)
add_custom_target(uninstall
    COMMAND xargs rm < install_manifest.txt
)