
list(APPEND files "main.cpp")

add_executable(fisk_input_builtin_devices_keyboard ${files})

target_link_libraries(fisk_input_builtin_devices_keyboard fisk_input_output)