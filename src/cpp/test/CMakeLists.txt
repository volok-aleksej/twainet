cmake_minimum_required(VERSION 2.6)
project(twntest)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
	Application.cpp
	test.cpp
	test_module.cpp
	../../messages/deamon.pb.cc
)

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_SOURCE_DIR}/..
		    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
		    ${CMAKE_CURRENT_SOURCE_DIR}/../external/udt4/src/
		    ${CMAKE_CURRENT_SOURCE_DIR}/../external/protobuf-2.5.0/src
		    ${CMAKE_CURRENT_SOURCE_DIR}/../../messages)
	    
add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../output/${PROJECT_NAME})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../output/work/${CMAKE_BUILD_TYPE})

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

target_link_libraries(${PROJECT_NAME} INTERFACE pthread)
