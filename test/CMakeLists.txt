link_libraries(check subunit m)
add_executable(test_preferred_value preferred_value.test.c)
target_link_libraries(test_preferred_value preferred_value)

add_executable(test_component component.test.c)
target_link_libraries(test_component component preferred_value random mtwister)

add_executable(test_load load.test.c)
target_link_libraries(test_load load component preferred_value random mtwister)

add_executable(test_filter filter.test.c)
target_link_libraries(test_filter filter load component preferred_value random mtwister two_port_network)