link_libraries(check subunit)
add_executable(test_preferred_value preferred_value.test.c)
target_link_libraries(test_preferred_value filterforge)

add_executable(test_component component.test.c)
target_link_libraries(test_component filterforge)

add_executable(test_load load.test.c)
target_link_libraries(test_load filterforge)

add_executable(test_filter filter.test.c)
target_link_libraries(test_filter filterforge)

add_executable(test_input config_input.test.c)
target_link_libraries(test_input filterforge)

configure_file(test_filter.json test_filter.json COPYONLY)