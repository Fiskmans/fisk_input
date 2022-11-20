cmake_minimum_required(VERSION 3.22)

project(FISK_INPUT)
set(CXX_STANDARD, 20)

#externals
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(fisk_tools)

#local
add_subdirectory(common)
add_subdirectory(input)
add_subdirectory(cpp_output)
