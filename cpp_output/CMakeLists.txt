
list(APPEND files "Output.cpp" "Output.h")

add_library(fisk_input_output ${files})

set_property(TARGET fisk_input_output PROPERTY CXX_STANDARD 20)

target_include_directories(fisk_input_output PUBLIC ${FISK_INCLUDE_DIRS})

add_dependencies(fisk_input_output fisk_input_common)