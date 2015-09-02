cmake_minimum_required(VERSION 2.6)
project(twndeamon)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
set (SRC_${PROJECT_NAME} 
	../../common/file.cpp
	../../common/process.cpp
	../../common/service_manager.cpp
	../../utils/path_parser.cpp
	../../utils/utils.cpp
	application/application.cpp 
	application/main.cpp 
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
	  find_library(TWAINET_LIBRARY  NAMES twainet PATHS ${LIB_PATH}) 
	  if(THREAD_LIBRARY AND TWAINET_LIBRARY) 
		set(EXTRA_LIBS ${THREAD_LIBRARY} ${TWAINET_LIBRARY}) 
		target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS}) 
	  endif(THREAD_LIBRARY AND TWAINET_LIBRARY) 
    else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 
	  add_dependencies(${PROJECT_NAME} thread_lib twainet) 
	  target_link_libraries(${PROJECT_NAME} thread_lib twainet) 
    endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 


    find_library(NTLM_LIBRARY NAMES libntlm.so.0 PATHS ${LIB_PATH} /usr/lib/x86_64-linux-gnu) 
    if(NTLM_LIBRARY) 
	  target_link_libraries(${PROJECT_NAME} ${NTLM_LIBRARY}) 
    endif(NTLM_LIBRARY) 

    target_link_libraries(${PROJECT_NAME} pthread)