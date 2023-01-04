list(APPEND files "Input.cpp")

add_executable(fisk_input_test ${files})

target_include_directories(fisk_input_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_input_test PUBLIC fisk_input)
target_link_libraries(fisk_input_test PUBLIC fisk_input_output)
target_link_libraries(fisk_input_test PRIVATE Catch2::Catch2WithMain)


catch_discover_tests(fisk_input_test)