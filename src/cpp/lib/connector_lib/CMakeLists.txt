cmake_minimum_required(VERSION 2.6)
project(connector_lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
    ../../common/common_func.cpp
    ../../utils/base64.cpp
    common/aes.cpp
    connector/connector.cpp
    connector/connector_manager.cpp
    handler/message_handler.cpp
    message/connector_messages.cpp
    signal/signal.cpp
    signal/signal_manager.cpp
    signal/signal_owner.cpp
    signal/signal_receiver.cpp
    socket/secure_socket.cpp
    socket/proxy_socket.cpp
    socket/socket_factories.cpp
    socket/tcp_socket.cpp
    socket/udp_socket.cpp
    socket/udt_socket.cpp
    thread/connect_thread.cpp
    thread/listen_thread.cpp
)

add_definitions(-fPIC)	

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_SOURCE_DIR}/..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src)

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
