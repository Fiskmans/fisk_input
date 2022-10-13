
list(APPEND files "InputEvent.cpp" "InputEvent.h")
list(APPEND files "Types.h")

add_library(fisk_input_common ${files})

target_include_directories(fisk_input_common PUBLIC ${FISK_INCLUDE_DIRS})

add_dependencies(fisk_input_common fisk_tools)