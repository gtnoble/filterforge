add_library(
    filterforge STATIC 
    preferred_value.c
    component.c
    random.c
    mtwister.c
    load.c
    filter.c
    two_port_network.c
    config_input.c
    config_output.c
    optimize.c
)
target_link_libraries(filterforge -lm -ljansson)