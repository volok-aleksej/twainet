cmake_minimum_required(VERSION 2.6)
project(protobuf)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
#    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set (SRC_${PROJECT_NAME}
    src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
    src/google/protobuf/stubs/common.cc
    src/google/protobuf/stubs/once.cc
    src/google/protobuf/stubs/stringprintf.cc
    src/google/protobuf/stubs/structurally_valid.cc
    src/google/protobuf/stubs/strutil.cc
    src/google/protobuf/stubs/substitute.cc
    src/google/protobuf/io/printer.cc
    src/google/protobuf/io/coded_stream.cc
    src/google/protobuf/io/gzip_stream.cc
    src/google/protobuf/io/tokenizer.cc
    src/google/protobuf/io/zero_copy_stream.cc
    src/google/protobuf/io/zero_copy_stream_impl.cc
    src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    src/google/protobuf/descriptor.cc
    src/google/protobuf/descriptor.pb.cc
    src/google/protobuf/descriptor_database.cc
    src/google/protobuf/dynamic_message.cc
    src/google/protobuf/extension_set.cc
    src/google/protobuf/extension_set_heavy.cc
    src/google/protobuf/generated_message_reflection.cc
    src/google/protobuf/generated_message_util.cc
    src/google/protobuf/message.cc
    src/google/protobuf/message_lite.cc
    src/google/protobuf/reflection_ops.cc
    src/google/protobuf/repeated_field.cc
    src/google/protobuf/service.cc
    src/google/protobuf/text_format.cc
    src/google/protobuf/unknown_field_set.cc
    src/google/protobuf/wire_format.cc
    src/google/protobuf/wire_format_lite.cc
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
		    ${CMAKE_CURRENT_SOURCE_DIR}/linux)

add_library (${PROJECT_NAME} STATIC ${SRC_${PROJECT_NAME}})
set(CMAKE_LIBRARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output/lib/${CMAKE_BUILD_TYPE})
