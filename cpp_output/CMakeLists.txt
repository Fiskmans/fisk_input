
list(APPEND files "Output.cpp" "Output.h")

add_library(fisk_input_output ${files})

target_link_libraries(fisk_input_output PUBLIC fisk_input_common)