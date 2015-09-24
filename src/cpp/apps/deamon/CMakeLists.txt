cmake_minimum_required(VERSION 2.6)
project(twndeamon)

execute_process(COMMAND
		protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
		       --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
		       ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.proto
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

add_custom_command(OUTPUT 
		  ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.pb.cc
		  ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.pb.h
		  PRE_BUILD COMMAND
		  protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
			 --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
			 ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.proto)

set (SRC_${PROJECT_NAME} 
	../../common/file.cpp
	../../common/process.cpp
	../../common/service_manager.cpp
	../../common/dir.cpp
	../../utils/path_parser.cpp
	../../utils/utils.cpp
	../../../messages/deamon.pb.cc
	application/application.cpp 
	application/main.cpp 
	module/deamon_module.cpp 
    ) 

    set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE} 
		 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work) 

    include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
			${CMAKE_CURRENT_SOURCE_DIR}/../.. 
			${CMAKE_CURRENT_SOURCE_DIR}/../../lib 
			${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src/ 
			${CMAKE_CURRENT_SOURCE_DIR}/../../../messages) 

    add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}}) 
    set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME}) 
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE}) 

    if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 
	  find_library(THREAD_LIBRARY  NAMES thread_lib PATHS ${LIB_PATH}) 
	  find_library(CONNECTOR_LIBRARY  NAMES connector_lib PATHS ${LIB_PATH}) 
	  find_library(TWAINET_LIBRARY  NAMES twainet PATHS ${LIB_PATH}) 
	  if(THREAD_LIBRARY AND TWAINET_LIBRARY AND CONNECTOR_LIBRARY) 
		set(EXTRA_LIBS ${THREAD_LIBRARY} ${CONNECTOR_LIBRARY} ${TWAINET_LIBRARY}) 
		target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS}) 
	  endif(THREAD_LIBRARY AND TWAINET_LIBRARY) 
    else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 
	  add_dependencies(${PROJECT_NAME} thread_lib connector_lib twainet) 
	  target_link_libraries(${PROJECT_NAME} thread_lib connector_lib twainet) 
    endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 


    find_library(NTLM_LIBRARY NAMES libntlm.so.0 PATHS ${LIB_PATH}) 
    if(NTLM_LIBRARY) 
	  target_link_libraries(${PROJECT_NAME} ${NTLM_LIBRARY}) 
    endif(NTLM_LIBRARY) 

    target_link_libraries(${PROJECT_NAME} pthread)