cmake_minimum_required(VERSION 2.6)
project(twainet)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(TWAINET_VERSION_MAJOR 1)
set(TWAINET_VERSION_MINOR 1)
set(TWAINET_VERSION_PATCH 0)
set(TWAINET_VERSION_STRING ${TWAINET_VERSION_MAJOR}.${TWAINET_VERSION_MINOR})

set (SRC_${PROJECT_NAME}
        ../../common/file.cpp
        ../../common/logger.cpp
        ../../utils/path_parser.cpp
	application/application.cpp
	application/twainet.cpp
	message/NotificationMessages.cpp
	module/twainet_module.cpp
)

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
set(DEPENDENCY_LIBS udt
		    thread_lib
		    connector_lib
		    ipc_lib
		    client_server_lib
		    tunnel_lib)

add_definitions(-fPIC)

if(NOT USE_SYSTEM_PROTOBUF)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)
else(NOT USE_SYSTEM_PROTOBUF)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src)
endif(NOT USE_SYSTEM_PROTOBUF)
		    
add_library (${PROJECT_NAME} SHARED ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE})

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})	
	find_library(UDT_LIBRARY  NAMES udt PATHS ${LIB_PATH})
	if(UDT_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${UDT_LIBRARY})
	endif(UDT_LIBRARY)
	
	find_library(THREAD_LIBRARY  NAMES thread_lib PATHS ${LIB_PATH})
	if(THREAD_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${THREAD_LIBRARY})
	endif(THREAD_LIBRARY)
	
	find_library(CONNECTOR_LIBRARY  NAMES connector_lib PATHS ${LIB_PATH})
	if(CONNECTOR_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${CONNECTOR_LIBRARY})
	endif(CONNECTOR_LIBRARY)
	
	find_library(IPC_LIBRARY  NAMES ipc_lib PATHS ${LIB_PATH})
	if(IPC_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${IPC_LIBRARY})
	endif(IPC_LIBRARY)
	
	find_library(CLIENT_SERVER_LIBRARY  NAMES client_server_lib PATHS ${LIB_PATH})
	if(CLIENT_SERVER_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${CLIENT_SERVER_LIBRARY})
	endif(CLIENT_SERVER_LIBRARY)
	
	find_library(TUNNEL_LIBRARY  NAMES tunnel_lib PATHS ${LIB_PATH})
	if(TUNNEL_LIBRARY)
	  set(EXTRA_LIBS ${EXTRA_LIBS} ${TUNNEL_LIBRARY})
	endif(TUNNEL_LIBRARY)
		
	target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-whole-archive ${EXTRA_LIBS} -Wl,-no-whole-archive)
else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	add_dependencies(${PROJECT_NAME} ${DEPENDENCY_LIBS})
	target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-whole-archive ${DEPENDENCY_LIBS} -Wl,-no-whole-archive)
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

find_library(NTLM_LIBRARY NAMES libntlm.so.0 PATHS ${LIB_PATH})
if(NTLM_LIBRARY)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${NTLM_LIBRARY})
endif(NTLM_LIBRARY)

if(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-whole-archive ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE}/libprotobuf.a -Wl,-no-whole-archive)
else(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE protobuf)
endif(NOT USE_SYSTEM_PROTOBUF)
  
target_link_libraries(${PROJECT_NAME} PRIVATE crypto uuid z pthread rt)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${TWAINET_VERSION_STRING})

install(TARGETS twainet
	LIBRARY DESTINATION /usr/lib)