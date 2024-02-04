
link_libraries(check subunit)

add_executable(test_preferred_value preferred_value.test.c)
target_link_libraries(test_preferred_value filterforge)
add_test(NAME TestPreferredValue COMMAND test_preferred_value)

add_executable(test_component component.test.c)
target_link_libraries(test_component filterforge)
add_test(NAME TestComponent COMMAND test_component)

add_executable(test_load load.test.c)
target_link_libraries(test_load filterforge)
add_test(NAME TestLoad COMMAND test_load)

add_executable(test_filter filter.test.c)
target_link_libraries(test_filter filterforge)
add_test(NAME TestFilter COMMAND test_filter)

add_executable(test_input config_input.test.c)
target_link_libraries(test_input filterforge)
add_test(NAME TestInput COMMAND test_input ./test_filter.json)

configure_file(test_filter.json test_filter.json COPYONLY)