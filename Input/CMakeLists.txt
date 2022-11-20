
list(APPEND files "Input.cpp" "Input.h")

add_library(fisk_input ${files})

set_property(TARGET fisk_input PROPERTY CXX_STANDARD 20)

target_include_directories(fisk_input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_input fisk_input_common)