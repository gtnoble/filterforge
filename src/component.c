#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <complex.h>
#include <libguile.h>

#include "component.h"
#include "preferred_value.h"
#include "random.h"

SCM component_type;

SCM make_component(
    SCM type, 
    SCM value, 
    SCM lower_limit, 
    SCM upper_limit, 
    SCM is_connected,
    SCM prng
);
SCM get_component_value(SCM component);
SCM get_component_type(SCM component);
SCM get_component_lower_limit(SCM component);
SCM get_component_upper_limit(SCM component);
SCM get_component_is_connected(SCM component);
SCM set_component_is_connected(SCM is_connected, SCM component);
SCM get_component_prng(SCM component);


void init_component_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    name = scm_from_utf8_symbol("component");
    slots = scm_list_n(
        scm_from_utf8_symbol("type"),
        scm_from_utf8_symbol("value"),
        scm_from_utf8_symbol("lower-limit"),
        scm_from_utf8_symbol("upper-limit"),
        scm_from_utf8_symbol("is-connected"),
        scm_from_utf8_symbol("random-number-generator"),
        SCM_UNDEFINED
    );
    finalizer = NULL;
    component_type = scm_make_foreign_object_type(name, slots, finalizer);

    __extension__
    scm_c_define_gsubr("make-component", 4, 0, 0, (scm_t_subr) make_component);
    __extension__
    scm_c_define_gsubr("get-component-value", 1, 0, 0, (scm_t_subr) get_component_value);
    __extension__
    scm_c_define_gsubr("get-component-lower-limit", 1, 0, 0, (scm_t_subr) get_component_lower_limit);
    __extension__
    scm_c_define_gsubr("get-component-upper-limit", 1, 0, 0, (scm_t_subr) get_component_upper_limit);
    __extension__
    scm_c_define_gsubr("component-random-update", 1, 0, 0, (scm_t_subr) component_random_update);
    __extension__
    scm_c_define_gsubr("component-connected?", 1, 0, 0, (scm_t_subr) get_component_is_connected);
    __extension__
    scm_c_define_gsubr("set-component-connected", 2, 0, 0, (scm_t_subr) set_component_is_connected);
    __extension__
    scm_c_define_gsubr("duplicate-component", 1, 0, 0, (scm_t_subr) duplicate_component);

}

SCM make_component(
    SCM type, 
    SCM value, 
    SCM lower_limit, 
    SCM upper_limit, 
    SCM is_connected,
    SCM prng
) {
    scm_assert_foreign_object_type(preferred_component_value_type, value);
    scm_assert_foreign_object_type(preferred_component_value_type, lower_limit);
    scm_assert_foreign_object_type(preferred_component_value_type, upper_limit);

    SCM component_fields[] = 
        {type, value, lower_limit, upper_limit, is_connected, prng};
    return scm_make_foreign_object_n(component_type, 6, (void **) component_fields);
}

SCM duplicate_component(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    return make_component(
        get_component_type(component),
        duplicate_preferred_component_value(get_component_value(component)),
        duplicate_preferred_component_value(
            get_component_lower_limit(component)
        ),
        duplicate_preferred_component_value(
            get_component_upper_limit(component)
        ),
        get_component_is_connected(component),
        get_component_prng(component)
    );
}

SCM get_component_type(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM type = scm_foreign_object_ref(component, 0);
    return type;
}

SCM get_component_value(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM value = scm_foreign_object_ref(component, 1);
    scm_assert_foreign_object_type(preferred_component_value_type, value);
    return value;
}

SCM get_component_lower_limit(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM lower_limit = scm_foreign_object_ref(component, 2);
    scm_assert_foreign_object_type(preferred_component_value_type, lower_limit);
    return lower_limit;
}

SCM get_component_upper_limit(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM upper_limit = scm_foreign_object_ref(component, 3);
    scm_assert_foreign_object_type(preferred_component_value_type, upper_limit);
    return upper_limit;
}

SCM get_component_is_connected(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM is_connected = scm_foreign_object_ref(component, 4);
    return is_connected;
}

SCM set_component_is_connected(SCM is_connected, SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    scm_foreign_object_set_x(component, 4, is_connected);
    return is_connected;
}

SCM get_component_prng(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    SCM prng = scm_foreign_object_ref(component, 5);
    return prng;
}

double complex component_impedance(double angular_frequency, SCM component) {
    assert(angular_frequency >= 0);
    scm_assert_foreign_object_type(component_type, component);

    if(scm_is_false(get_component_is_connected(component))) {
        return INFINITY;
    }

    SCM type = get_component_type(component);
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

SCM component_random_update(SCM component) {
    scm_assert_foreign_object_type(component_type, component);

    SCM value = get_component_value(component);
    SCM min_range = get_component_lower_limit(component);
    SCM max_range = get_component_upper_limit(component);

    scm_assert_foreign_object_type(preferred_component_value_type, value);
    scm_assert_foreign_object_type(preferred_component_value_type, min_range);
    scm_assert_foreign_object_type(preferred_component_value_type, max_range);

    SCM prng = get_component_prng(component);

    if (gen_random_bool(prng)) {
        set_component_is_connected(SCM_BOOL_T, component);
    }
    else {
        set_component_is_connected(SCM_BOOL_F, component);
    }

    if (component_values_equal(value, min_range)) {
        return increment_component_value(value);
    }
    else if (component_values_equal(value, max_range)) {
        return decrement_component_value(value);
    }
    else {
        if(gen_random_bool(prng)) {
            decrement_component_value(value);
        } 
        else {
            increment_component_value(value);
        }
    }
    return value;
}





