cmake_minimum_required(VERSION 3.22)

project(FISK_INPUT)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

#externals
add_subdirectory(fisk_tools)

#local
add_subdirectory(fisk_input_common)
add_subdirectory(fisk_input)
add_subdirectory(cpp_output)
add_subdirectory(builtin)


Include(CTest)
Include(FetchContent)

if(BUILD_TESTING)
	FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        v3.0.1 # or a later release
	)

	FetchContent_MakeAvailable(Catch2)

	add_subdirectory(input_test)
endif()