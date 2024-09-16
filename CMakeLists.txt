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
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  include(CTest)
  enable_testing()

  add_executable(TestSymbols Test.c)
  target_link_libraries(TestSymbols PRIVATE GlibcBt)
  set_property(TARGET TestSymbols PROPERTY ENABLE_EXPORTS ON)

  add_executable(TestNoSymbols Test.c)
  target_link_libraries(TestNoSymbols PRIVATE GlibcBt)
  target_link_options(TestNoSymbols PRIVATE -static)

  add_test(NAME TestSymbols COMMAND "$<TARGET_FILE:TestSymbols>")
  add_test(NAME TestNoSymbols COMMAND "$<TARGET_FILE:TestNoSymbols>")
endif()

# ClangForceFormat target
add_custom_target(ClangForceFormat
COMMAND clang-format
  -i
  *.c *.h
WORKING_DIRECTORY
  ${CMAKE_SOURCE_DIR}
COMMENT
  "Formatting with clang-format ..."
)
