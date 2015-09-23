cmake_minimum_required(VERSION 2.6)
project(tunnel_lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
	../../common/network.cpp
	connector/tunnel_connector.cpp
	connector/tunnel_server_connector.cpp
	message/tunnel_messages.cpp
	module/client_signal_handler.cpp
	module/server_signal_handler.cpp
	module/tunnel_containers.cpp
	module/tunnel_module.cpp
	thread/external_connect_thread.cpp
	thread/external_listen_thread.cpp
	thread/external_recv_thread.cpp
	thread/relay_listen_thread.cpp
	thread/tunnel_checker_thread.cpp
)

add_definitions(-fPIC)	

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_SOURCE_DIR}/..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
