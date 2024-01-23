link_libraries(check subunit)
add_executable(test_preferred_value preferred_value.test.c)
target_link_libraries(test_preferred_value preferred_value m)

add_executable(test_component component.test.c)
target_link_libraries(test_component component preferred_value random mtwister m)

add_executable(test_load load.test.c)
target_link_libraries(test_load load component preferred_value random mtwister m)