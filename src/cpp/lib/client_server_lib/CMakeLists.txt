cmake_minimum_required(VERSION 2.6)
project(client_server_lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/protoc)
    execute_process(COMMAND
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)
else(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH protoc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..)
endif(NOT USE_SYSTEM_PROTOBUF)

set (SRC_${PROJECT_NAME}
    connector/client_server_connector.cpp
    message/client_server_messages.cpp
    module/client_server_module.cpp
    module/client_server_signal_handler.cpp
)

set (SRC_GEN_${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.pb.cc
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.pb.h)

set_source_files_properties(${SRC_GEN_${PROJECT_NAME}} PROPERTIES GENERATED TRUE)
add_custom_command( OUTPUT ${SRC_GEN_${PROJECT_NAME}}
                    COMMAND ${PROTOC_PATH}
                    ARGS --proto_path=. --cpp_out=. client_server.proto
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                    DEPENDS ../../../messages/client_server.proto)
                    
add_definitions(-fPIC)	

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}} ${SRC_GEN_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
