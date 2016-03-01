cmake_minimum_required(VERSION 2.6)
project(tilc)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
    FILE(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
    SET(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")
    ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
      COMMAND ld -r -b binary -o ${out_f} ${src_f}
      DEPENDS ${in_f}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Building GLSL object ${out_f}"
      VERBATIM
      )
    LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set (SRC_${PROJECT_NAME}
	../../common/file.cpp
	../../utils/path_parser.cpp
	main.cpp
	types.cpp
	compiler/til_compiler.cpp
    compiler/tokenizer.cpp
    compiler/states/compiler_root_state.cpp
    compiler/states/compiler_comment_state.cpp
    compiler/states/compiler_plugin_app_state.cpp
    compiler/states/compiler_var_state.cpp
    compiler/states/compiler_function_state.cpp
    compiler/states/compiler_function_attr_state.cpp
    compiler/states/compiler_module_state.cpp
    generator/generator_manager.cpp
    generator/generators/til_generator.cpp
    generator/generators/generator_helper.cpp
    generator/generators/app_generator.cpp
    generator/generators/module_generator.cpp
    generator/generators/function_generator.cpp
    generator/generators/var_generator.cpp
    )

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/resources)
file(COPY ../../../messages/deamon.proto DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/resources)
ADD_RESOURCES(someResources 
              resources/application.h.tmpl
              resources/application.cpp.tmpl
              resources/module.h.tmpl
              resources/module.proto.tmpl
              resources/deamon.proto)

add_executable (${PROJECT_NAME} ${SRC_${PROJECT_NAME}} ${someResources})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/work/${CMAKE_BUILD_TYPE}) 

install(TARGETS tilc
	RUNTIME DESTINATION /usr/bin)