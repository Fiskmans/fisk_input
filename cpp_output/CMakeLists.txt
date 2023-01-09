
list(APPEND files "Output.cpp" "Output.h")

add_library(fisk_input_output ${files})

set_property(TARGET fisk_input_output PROPERTY CXX_STANDARD 20)

target_link_libraries(fisk_input_output PUBLIC fisk_input_common)