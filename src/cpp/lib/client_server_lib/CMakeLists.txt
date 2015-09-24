cmake_minimum_required(VERSION 2.6)
project(client_server_lib)

execute_process(COMMAND
		protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
		       --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
		       ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.proto
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_custom_command(OUTPUT 
		  ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.pb.cc
		  ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.pb.h
		  PRE_BUILD COMMAND
		  protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
			 --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
			 ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/client_server.proto)
			 
set (SRC_${PROJECT_NAME}
    ../../../messages/client_server.pb.cc
    connector/client_server_connector.cpp
    message/client_server_messages.cpp
    module/client_server_module.cpp
    module/client_server_signal_handler.cpp
)

add_definitions(-fPIC)	

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_SOURCE_DIR}/..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
