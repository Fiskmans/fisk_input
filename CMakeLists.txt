cmake_minimum_required(VERSION 3.22)

project(FISK_INPUT)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

Include(FetchContent)

FetchContent_Declare(
  fisk_tools
  GIT_REPOSITORY https://github.com/Fiskmans/fisk_input.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(fisk_tools)

add_subdirectory(fisk_input_common)
add_subdirectory(fisk_input)
add_subdirectory(cpp_output)
add_subdirectory(builtin)


Include(CTest)
	
if(BUILD_TESTING)
	FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        v3.0.1 # or a later release
	)

	FetchContent_MakeAvailable(Catch2)

	add_subdirectory(input_test)
endif()