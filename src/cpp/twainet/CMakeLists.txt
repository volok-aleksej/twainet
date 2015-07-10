cmake_minimum_required(VERSION 2.6)
project(twainet)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
	application/application.cpp
	application/twainet.cpp
	message/NotificationMessages.cpp
	module/twainet_module.cpp
)

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_SOURCE_DIR}/..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
		    ${CMAKE_CURRENT_SOURCE_DIR}/../external/udt4/src/
		    ${CMAKE_CURRENT_SOURCE_DIR}/../external/protobuf-2.5.0/src
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../messages)

add_library (${PROJECT_NAME} SHARED ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../output/work)

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	find_library(PUBLISH_LIBRARY  NAMES PUBLISH PATHS /usr/lib /usr/local/lib ${LIB_PATH})
	if(PUBLISH_LIBRARY)
	  set(EXTRA_LIBS ${PUBLISH_LIBRARY})
	  target_link_libraries(${PROJECT_NAME} ${PUBLISH_LIBRARY} pthread)
	endif(PUBLISH_LIBRARY)
else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	add_dependencies(${PROJECT_NAME} udt protobuf thread_lib connector_lib ipc_lib client_server_lib tunnel_lib)
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})