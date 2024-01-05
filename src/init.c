#include <libguile.h>

#include "component.h"
#include "filter.h"
#include "load.h"
#include "preferred_value.h"
#include "two_port_network.h"

void init_filtopt() {

}

void *guile_allocate(size_t size) {
    return scm_gc_malloc(size, "Guile GC Filtopt allocated data");
}

void guile_free(void *) {
    return;
}

FilterType guile_to_filter_type(SCM filter_type) {
    if (scm_is_eq(filter_type, scm_from_utf8_symbol("series")))
        return SERIES_FILTER;
    else if (scm_is_eq(filter_type, scm_from_utf8_symbol("shunt")))
        return SHUNT_FILTER;
    else {
        assert(false);
        return;
    }
}

SCM guile_make_filter_stage(SCM type, SCM load) {
    return scm_from_pointer(
        make_filter_stage(
            scm_to_pointer(load), 
            guile_to_filter_type(type), 
            guile_allocate
        ), 
        NULL);
}

ComponentType guile_to_component_type(SCM component_type) {
    if (scm_is_eq(component_type, scm_from_utf8_symbol("resistor")))
        return RESISTOR;
    else if (scm_is_eq(component_type, scm_from_utf8_symbol("capacitor")))
        return CAPACITOR;
    else if (scm_is_eq(component_type, scm_from_utf8_symbol("inductor")))
        return INDUCTOR;
    else {
        assert(false);
        return;
    }
}

SCM guile_make_nearest_component(SCM component_type, SCM value, SCM lower_limit, SCM upper_limit, SCM is_connected) {
    PreferredValue *lower_limit_preferred = make_preferred_value(0, 0, guile_allocate);
    floor_preferred_value(scm_to_double(lower_limit), lower_limit_preferred);

    PreferredValue *upper_limit_preferred = make_preferred_value(0, 0, guile_allocate);
    ceiling_preferred_value(scm_to_double(upper_limit), upper_limit_preferred);

    PreferredValue *value_preferred = make_preferred_value(0, 0, guile_allocate);
    nearest_preferred_value(scm_to_double(value), value_preferred);

    return scm_from_pointer(
        make_component(
            guile_to_component_type(component_type),
            value_preferred,
            lower_limit_preferred,
            upper_limit_preferred,
            scm_to_bool(is_connected),
            guile_allocate
        ),
        NULL
    );
}

SCM guile_make_component_load(SCM component) {
    return scm_from_pointer(make_component_load(scm_to_pointer(component), guile_allocate), NULL);
}

SCM guile_make_series_load(SCM loads) {
    Load *loads_array[] = guile_allocate(sizeof(Load) * SCM_SIMPLE_VECTOR_LENGTH(loads));
    for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(loads); i++) {
        loads_array[i] = scm_to_pointer(SCM_SIMPLE_VECTOR_REF(loads, i));
    }
    return scm_from_pointer(make_series_load(loads_array, guile_allocate), NULL);
}

SCM guile_make_parallel_load(SCM loads) {
    Load *loads_array[] = guile_allocate(sizeof(Load) * SCM_SIMPLE_VECTOR_LENGTH(loads));
    for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(loads); i++) {
        loads_array[i] = scm_to_pointer(SCM_SIMPLE_VECTOR_REF(loads, i));
    }
    return scm_from_pointer(make_parallel_load(loads_array, guile_allocate), NULL);
}

