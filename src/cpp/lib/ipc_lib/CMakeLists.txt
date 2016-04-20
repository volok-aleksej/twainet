cmake_minimum_required(VERSION 2.6)
project(ipc_lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/protoc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)
else(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH protoc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages)
endif(NOT USE_SYSTEM_PROTOBUF)

set (SRC_${PROJECT_NAME}
    ../../common/tstring.cpp
    ../../common/user.cpp
    ../../utils/utils.cpp
    connector/ipc_connector.cpp
    connector/ipc_handler.cpp
    connector/internal_connector.cpp
    module/ipc_module.cpp
    module/ipc_object_name.cpp
    module/ipc_signal_handler.cpp
    thread/ipc_checker_thread.cpp
    thread/ping_thread.cpp
)

set (SRC_GEN_${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/ipc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/ipc.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/interconn.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/interconn.pb.h)

set_source_files_properties(${SRC_GEN_${PROJECT_NAME}} PROPERTIES GENERATED TRUE)
add_custom_command( OUTPUT ${SRC_GEN_${PROJECT_NAME}}
                    COMMAND ${PROTOC_PATH}
                    ARGS --proto_path=. --cpp_out=. ipc.proto interconn.proto
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                    DEPENDS ../../../messages/ipc.proto ../../../messages/interconn.proto)

add_definitions(-fPIC)	

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}} ${SRC_GEN_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
