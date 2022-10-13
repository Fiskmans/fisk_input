
list(APPEND files "Input.cpp" "Input.h")

add_library(fisk_input ${files})

set_property(TARGET fisk_input PROPERTY CXX_STANDARD 20)

target_include_directories(fisk_input PUBLIC ${FISK_INCLUDE_DIRS})

add_dependencies(fisk_input fisk_input_common )