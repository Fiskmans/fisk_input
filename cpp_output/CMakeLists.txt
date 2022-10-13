
list(APPEND files "Output.cpp" "Output.h")

add_library(fisk_input_output ${files})

target_include_directories(fisk_input_output PUBLIC ${FISK_INCLUDE_DIRS})

add_dependencies(fisk_input_output fisk_input_common)