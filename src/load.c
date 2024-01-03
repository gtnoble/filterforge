#include <complex.h>
#include <stdio.h>
#include <assert.h>
#include <libguile.h>
#include <stdbool.h>

#include "load.h"
#include "component.h"

SCM load_type;
SCM component_load_symbol;
SCM series_load_symbol;
SCM parallel_load_symbol;

SCM make_component_load(SCM component);
SCM make_series_load(SCM loads);
SCM make_parallel_load(SCM loads);
SCM scm_load_impedance(SCM angular_frequency, SCM load);
void invalid_load_type_error(void);


void init_load_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    component_load_symbol = scm_from_utf8_symbol("component");
    series_load_symbol = scm_from_utf8_symbol("series");
    parallel_load_symbol = scm_from_utf8_symbol("parallel");

    name = scm_from_utf8_symbol("load");
    slots = scm_list_2(
        scm_from_utf8_symbol("type"),
        scm_from_utf8_symbol("elements")
    );
    finalizer = NULL;
    load_type = scm_make_foreign_object_type(name, slots, finalizer);

    __extension__
    scm_c_define_gsubr("make-component-load", 1, 0, 0, (scm_t_subr) make_component_load);
    __extension__
    scm_c_define_gsubr("make-series-load", 1, 0, 0, (scm_t_subr) make_series_load);
    __extension__
    scm_c_define_gsubr("make-parallel-load", 1, 0, 0, (scm_t_subr) make_parallel_load);
    __extension__
    scm_c_define_gsubr("impedance", 2, 0, 0, (scm_t_subr) scm_load_impedance);
}

SCM make_component_load(SCM component) {
    scm_assert_foreign_object_type(component_type, component);
    return scm_make_foreign_object_2(
        load_type, 
        scm_from_utf8_symbol("component"), 
        component
    );
}

SCM make_series_load(SCM loads) {
    SCM_ASSERT_TYPE(
        scm_is_vector(loads), 
        loads, 
        0, 
        "make-series-load", 
        "Vector of loads");
    return scm_make_foreign_object_2(
        load_type,
        scm_from_utf8_symbol("series"),
        loads
    );
}

SCM make_parallel_load(SCM loads) {
    SCM_ASSERT_TYPE(
        scm_is_vector(loads), 
        loads, 
        0, 
        "make-parallel-load", 
        "Vector of loads");
    return scm_make_foreign_object_2(
        load_type,
        scm_from_utf8_symbol("parallel"),
        loads
    );
}

SCM get_load_type(SCM load) {
    scm_assert_foreign_object_type(load_type, load);
    return scm_foreign_object_ref(load, 0);
}

SCM get_load_elements(SCM load) {
    scm_assert_foreign_object_type(load_type, load);
    return scm_foreign_object_ref(load, 1);
}

SCM load_random_update(SCM load) {
    scm_assert_foreign_object_type(load_type, load);
    SCM type = get_load_type(load);
    SCM elements = get_load_elements(load);

    bool is_component_load = scm_is_eq(type, component_load_symbol);
    bool is_parallel_load = scm_is_eq(type, parallel_load_symbol);
    bool is_series_load = scm_is_eq(type, series_load_symbol);

    if (is_component_load) {
        component_random_update(elements);
    }
    else if (is_parallel_load || is_series_load) {
        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(elements); i++) {
            load_random_update(SCM_SIMPLE_VECTOR_REF(elements, i));
        }
    }
    else {
        invalid_load_type_error();
    }
    return load;
}

SCM duplicate_load(SCM load) {
    scm_assert_foreign_object_type(load_type, load);
    SCM type = get_load_type(load);
    SCM elements = get_load_elements(load);

    bool is_component_load = scm_is_eq(type, component_load_symbol);
    bool is_parallel_load = scm_is_eq(type, parallel_load_symbol);
    bool is_series_load = scm_is_eq(type, series_load_symbol);

    SCM duplicated_load;
    if (is_component_load) {
        duplicated_load = make_component_load(duplicate_component(elements));
    }
    else if (is_series_load || is_parallel_load) {
        SCM next_loads = scm_c_make_vector(
            SCM_SIMPLE_VECTOR_LENGTH(elements), NULL
        );

        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(next_loads); i++) {
            SCM_SIMPLE_VECTOR_SET(
                next_loads, 
                i, 
                duplicate_load(SCM_SIMPLE_VECTOR_REF(elements, i))
            );
        }
        if (is_series_load) {
            duplicated_load = make_series_load(next_loads);
        }
        if (is_parallel_load) {
            duplicated_load = make_parallel_load(next_loads);
        }

    }
    else {
        invalid_load_type_error();
    }
    return duplicated_load;

}

void invalid_load_type_error(void) {
    scm_error_scm(
        scm_from_utf8_string("invalid-load-type"), 
        SCM_BOOL_F, 
        scm_from_utf8_string("Invalid load type."),
        SCM_BOOL_F,
        SCM_BOOL_F
    );
}


double complex load_impedance(double angular_frequency, SCM load) {
    assert(angular_frequency >= 0);

    scm_assert_foreign_object_type(load_type, load);
    SCM type = scm_foreign_object_ref(load, 0);
    SCM elements = scm_foreign_object_ref(load, 1);

    double complex impedance;
    if (scm_is_eq(type, scm_from_utf8_symbol("component"))) {
        impedance = component_impedance(angular_frequency, elements);
    }
    else if (scm_is_eq(type, scm_from_utf8_symbol("series"))) {
        double complex sumImpedance = 0;
        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(elements); i++) {
            SCM element = SCM_SIMPLE_VECTOR_REF(elements, i);
            sumImpedance += load_impedance(angular_frequency, element);
        }
        impedance = sumImpedance;
    }
    else if (scm_is_eq(type, scm_from_utf8_symbol("parallel"))) {
        double complex intermediate_impedance = 0;
        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(elements); i++) {
            SCM element = SCM_SIMPLE_VECTOR_REF(elements, i);
            intermediate_impedance += 1.0 / load_impedance(angular_frequency, element);
        }
        impedance = 1.0 / intermediate_impedance;
    }
    else {
        invalid_load_type_error();
    }
    return impedance;
}

SCM scm_load_impedance(SCM angular_frequency, SCM load) {
    scm_assert_foreign_object_type(load_type, load);
    return scm_from_double(
        load_impedance(scm_to_double(angular_frequency), load));
}

double complex admittance(double angular_frequency, SCM load) {
    assert(angular_frequency >= 0);

    return 1.0 / load_impedance(angular_frequency, load);
}

