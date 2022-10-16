cmake_minimum_required(VERSION 3.22)

if(FISK_INPUT_INCLUDED)
  return()
endif()
set(FISK_INPUT_INCLUDED true)

project(FISK_INPUT)
set(CXX_STANDARD, 20)

list(APPEND FISK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

#externals
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(fisk_tools)

#local
add_subdirectory(common)
add_subdirectory(input)
add_subdirectory(cpp_output)
