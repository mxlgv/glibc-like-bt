# SPDX-License-Identifier: MIT
# Copyright (c) 2024 Maxim Logaev

cmake_minimum_required(VERSION 3.5)
project(GlibcLikeBacktrace C ASM_NASM)

set(CMAKE_C_STANDARD 99)
include_directories(${CMAKE_SOURCE_DIR})

# Main libGBT target
add_library(GlibcBt STATIC GlibcBt.c FrameAddr.asm)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(GlibcBt PRIVATE 
        $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Wpedantic -Werror -fanalyzer>
    )
endif()

# Test target
include(CTest)
enable_testing()

add_executable(TestBacktrace tests/TestBacktrace.c)
target_link_libraries(TestBacktrace PRIVATE GlibcBt )
target_compile_definitions(TestBacktrace PRIVATE DEPTH=4)
add_test(NAME TestBacktrace COMMAND "$<TARGET_FILE:TestBacktrace>")

add_executable(TestBacktraceOver tests/TestBacktrace.c)
target_link_libraries(TestBacktraceOver PRIVATE GlibcBt)
target_compile_definitions(TestBacktraceOver PRIVATE DEPTH=40)
add_test(NAME TestBacktraceOver COMMAND "$<TARGET_FILE:TestBacktraceOver>")

# ClangForceFormat target
add_custom_target(ClangForceFormat
COMMAND clang-format
  -i
  *.c *.h */*.c */*.h
WORKING_DIRECTORY
  ${CMAKE_SOURCE_DIR}
COMMENT
  "Formatting with clang-format ..."
)
