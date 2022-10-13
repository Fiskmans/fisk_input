cmake_minimum_required(VERSION 3.22)

project(FISK_INPUT)
set(CXX_STANDARD, 20)

list(APPEND FISK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(input)
add_subdirectory(cpp_output)
