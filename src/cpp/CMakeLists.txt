cmake_minimum_required(VERSION 2.6)
project(twainet)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT USE_SYSTEM_PROTOBUF)
#    set(USE_SYSTEM_PROTOBUF 0)
    set(USE_SYSTEM_PROTOBUF 1)
endif(NOT USE_SYSTEM_PROTOBUF)

add_subdirectory(lib)
add_subdirectory(external/udt4)
add_subdirectory(apps)
add_subdirectory(test)

add_custom_target(pre-build
    COMMAND ./install.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/protobuf-2.5.0
)

add_custom_target(uninstall
    COMMAND xargs rm < install_manifest.txt
)

add_custom_target(clean-all
    COMMAND rm -r *
)
