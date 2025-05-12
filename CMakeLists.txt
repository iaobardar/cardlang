cmake_minimum_required(VERSION 3.10)

project(CardLang C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB SOURCES "src/*.c")

add_executable(CardLang ${SOURCES})

include_directories(include)

# Optional: Set compiler warnings
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(CardLang PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(CardLang PRIVATE /W4)
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)