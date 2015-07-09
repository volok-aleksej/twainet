cmake_minimum_required(VERSION 2.6)
project(thread_lib)

set(BUILD_64_TARGET 1)

set (SRC_${PROJECT_NAME}
    common/critical_section.cpp
    common/managers_container.cpp
    thread/thread.cpp
    thread/thread_impl.cpp
    thread/thread_manager.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)

#add_executable(${PROJECT_NAME} ${SRC_${PROJECT_NAME}})
#set(CMAKE_EXECUTABLE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work)
