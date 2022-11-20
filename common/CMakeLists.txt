
list(APPEND files "InputEvent.cpp" "InputEvent.h")
list(APPEND files "Types.h")

add_library(fisk_input_common ${files})

set_property(TARGET fisk_input_common PROPERTY CXX_STANDARD 20)

target_include_directories(fisk_input_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_input_common fisk_tools)