#include <complex.h>
#include <stdio.h>
#include <assert.h>

#include "load.h"
#include "component.h"

SCM load_type;

SCM make_component_load(SCM component);
SCM make_series_load(SCM loads);
SCM make_parallel_load(SCM loads);

void init_load_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    name = scm_from_utf8_symbol("load");
    slots = scm_list_2(
        scm_from_utf8_symbol("type"),
        scm_from_utf8_symbol("elements")
    );
    finalizer = NULL;
    load_type = scm_make_foreign_object_type(name, slots, finalizer);

    scm_c_define_gsubr("make-component-load", 1, 0, 0, make_component_load);
    scm_c_define_gsubr("make-series-load", 1, 0, 0, make_series_load);
    scm_c_define_gsubr("make-parallel-load", 1, 0, 0, make_parallel_load);
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
    return scm_make_foreign_object_2(
        load_type,
        scm_from_utf8_symbol("series"),
        loads
    );
}

SCM make_parallel_load(SCM loads) {
    return scm_make_foreign_object_2(
        load_type,
        scm_from_utf8_symbol("parallel"),
        loads
    );
}


double complex load_impedance(double angular_frequency, SCM load) {
    assert(angular_frequency >= 0);

    scm_assert_foreign_object_type(load_type, load);
    SCM type = scm_foreign_object_ref(load, 0);
    SCM elements = scm_foreign_object_ref(load, 1);

    if (scm_eq_p(type, scm_from_utf8_string("component"))) {
        scm_assert_foreign_object_type(component_type, elements);
        return component_impedance(angular_frequency, elements);
    }
    else if (scm_eq_p(type, scm_from_utf8_string("series"))) {
        double complex sumImpedance = 0;
        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(elements); i++) {
            SCM element = SCM_SIMPLE_VECTOR_REF(elements, i);
            scm_assert_foreign_object_type(load_type, load);
            sumImpedance += load_impedance(angular_frequency, element);
        }
        return sumImpedance;
    }
    else if (scm_eq_p(type, scm_from_utf8_string("parallel"))) {
        double complex intermediate_impedance = 0;
        for (size_t i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(elements); i++) {
            SCM element = SCM_SIMPLE_VECTOR_REF(elements, i);
            scm_assert_foreign_object_type(load_type, load);
            intermediate_impedance += 1.0 / load_impedance(angular_frequency, load);
        }
        return 1.0 / intermediate_impedance;
    }
    else {
        scm_error_scm(
            scm_from_utf8_string("invalid-load-type"), 
            SCM_BOOL_F, 
            scm_from_utf8_string("Invalid load type."),
            SCM_BOOL_F,
            SCM_BOOL_F
        );
    }
}

double complex admittance(double angular_frequency, SCM load) {
    assert(angular_frequency >= 0);

    return 1.0 / load_impedance(angular_frequency, load);
}

