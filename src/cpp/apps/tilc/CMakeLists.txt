cmake_minimum_required(VERSION 2.6)
project(tilc)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set (SRC_${PROJECT_NAME}
	../../common/file.cpp
	../../utils/path_parser.cpp
	main.cpp
	compiler/til_compiler.cpp
    compiler/tokenizer.cpp
    compiler/states/compiler_root_state.cpp
    compiler/states/compiler_comment_state.cpp
    compiler/states/compiler_plugin_app_state.cpp
    compiler/states/compiler_var_state.cpp
    compiler/states/compiler_function_state.cpp
    compiler/states/compiler_function_attr_state.cpp
    compiler/states/compiler_module_state.cpp
    )

add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE}) 

install(TARGETS tilc
	RUNTIME DESTINATION /usr/bin)