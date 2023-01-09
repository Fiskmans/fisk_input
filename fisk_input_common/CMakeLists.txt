
list(APPEND files "Types.h")
list(APPEND files "Dummy.cpp")

add_library(fisk_input_common ${files})

set_property(TARGET fisk_input_common PROPERTY CXX_STANDARD 20)

target_include_directories(fisk_input_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_input_common PUBLIC fisk_tools)