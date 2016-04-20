cmake_minimum_required(VERSION 2.6)
project(twndeamon)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT USE_SYSTEM_PROTOBUF)
    execute_process(COMMAND
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                                                                         --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                                                                         ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.proto
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/protobuf-2.5.0/src)
else(NOT USE_SYSTEM_PROTOBUF)
    execute_process(COMMAND
                    protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                           --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/../../../messages
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../../messages/deamon.proto
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../.. 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/udt4/src)
endif(NOT USE_SYSTEM_PROTOBUF)
		   
set (SRC_${PROJECT_NAME} 
	../../common/common_func.cpp
	../../common/file.cpp
	../../common/process.cpp
	../../common/service_manager.cpp
	../../common/dir.cpp
	../../common/ini_file.cpp
	../../../messages/deamon.pb.cc
	application/application.cpp 
	application/main.cpp 
	application/config.cpp 
	module/deamon_module.cpp 
    ) 

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE})  

add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}}) 
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME}) 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE}) 

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  find_library(TWAINET_LIBRARY  NAMES twainet PATHS ${LIB_PATH})
  if(TWAINET_LIBRARY) 
	set(EXTRA_LIBS ${TWAINET_LIBRARY}) 
	target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS}) 
  endif(THREAD_LIBRARY AND TWAINET_LIBRARY) 
else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  target_link_libraries(${PROJECT_NAME} PRIVATE twainet)
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}) 

target_link_libraries(${PROJECT_NAME} PRIVATE uuid pthread)
if(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE}/libprotobuf.a)
else(NOT USE_SYSTEM_PROTOBUF)
  target_link_libraries(${PROJECT_NAME} PRIVATE protobuf)
endif(NOT USE_SYSTEM_PROTOBUF)

install(TARGETS twndeamon
	RUNTIME DESTINATION /usr/bin)
install(FILES twndeamon
	PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	DESTINATION /etc/init.d)
install(FILES twndeamon.service
	DESTINATION /lib/systemd/system)