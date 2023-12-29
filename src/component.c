#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <complex.h>
#include <libguile.h>

#include "component.h"
#include "preferred_value.h"

void random_update(
    SCM value, SCM range
);

SCM component_type;
static SCM component_value_range_type;

void init_component_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    name = scm_from_utf8_symbol("component");
    slots = scm_list_4(
        scm_from_utf8_symbol("type"),
        scm_from_utf8_symbol("value"),
        scm_from_utf8_symbol("lower-limit"),
        scm_from_utf8_symbol("upper-limit")
    );
    finalizer = NULL;
    component_type = scm_make_foreign_object_type(name, slots, finalizer);
}

SCM make_component(SCM type, SCM value, SCM limits) {
    return scm_make_foreign_object_3(component_type, type, value, limits);
}

SCM get_component_value(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM value = scm_foreign_object_ref(component, 1);
    scm_assert_foreign_object_type(preferred_component_value_type, value);
    return value;
}

SCM get_component_limits(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM limits = scm_foreign_object_ref(component, 2);
    scm_assert_foreign_object_type(component_value_range_type, limits);
    return limits;
}

void random_component_update(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM value = get_component_value(component);
    SCM limits = get_component_limits(component);
    random_update(value, limits);
}


double complex component_impedance(double angular_frequency, SCM component) {
    assert(angular_frequency >= 0);
    scm_assert_foreign_object_type(component_type, component);

    SCM type = scm_foreign_object_ref(component, 0);
    double value = evaluated_component_value(get_component_value(component));

    if (scm_eq_p(type, scm_from_utf8_symbol("resistor"))) {
        return value;
    }
    else if (scm_eq_p(type, scm_from_utf8_symbol("capacitor"))) {
        return 1.0 / (I * angular_frequency * value);
    }
    else if (scm_eq_p(type, scm_from_utf8_symbol("inductor"))) {
        return I * angular_frequency * value;
    }
    else {
        scm_error_scm(
            scm_from_utf8_string("invalid-component-type"), 
            SCM_BOOL_F, 
            scm_from_utf8_string("Invalid component type."),
            SCM_BOOL_F,
            SCM_BOOL_F
        );
    }
}







