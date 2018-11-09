cmake_minimum_required(VERSION 2.6)
project(remote_terminal)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
	application/application.cpp
	application/main.cpp
	module/terminal_module.cpp
)

set (SRC_GEN_${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/messages/terminal.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/messages/terminal.pb.h)

add_definitions(-fPIC)
add_definitions(-DLIGTH_VERSION)

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE})

if(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/protoc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/udt4/src/
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)
else(NOT USE_SYSTEM_PROTOBUF)
    set(PROTOC_PATH protoc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src/)
endif(NOT USE_SYSTEM_PROTOBUF)

set_source_files_properties(${SRC_GEN_${PROJECT_NAME}} PROPERTIES GENERATED TRUE)
add_custom_command( OUTPUT ${SRC_GEN_${PROJECT_NAME}}
                    COMMAND ${PROTOC_PATH}
                    ARGS --proto_path=. --cpp_out=. terminal.proto
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/messages
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/messages/terminal.proto)

add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}} ${SRC_GEN_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE})

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	find_library(TWAINET_LIBRARY  NAMES twainet PATHS ${LIB_PATH})
	if(TWAINET_LIBRARY)
	  set(EXTRA_LIBS ${TWAINET_LIBRARY})
	  target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})
	endif(THREAD_LIBRARY AND TWAINET_LIBRARY)
else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	add_dependencies(${PROJECT_NAME} twainet)
	target_link_libraries(${PROJECT_NAME} PRIVATE twainet)
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
if(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../output/lib/${CMAKE_BUILD_TYPE}/libprotobuf.a)
else(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE protobuf)
endif(NOT USE_SYSTEM_PROTOBUF)
