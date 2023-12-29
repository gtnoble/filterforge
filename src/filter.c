#include <assert.h>
#include <stdlib.h>

#include "load.h"
#include "two_port_network.h"
#include "filter.h"

SCM filter_stage_type;

void init_filter_stage_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    name = scm_from_utf8_symbol("filter-stage");
    slots = scm_list_2(
        scm_from_utf8_symbol("type"),
        scm_from_utf8_symbol("load")
    );
    finalizer = NULL;
    filter_stage_type = scm_make_foreign_object_type(name, slots, finalizer);
}

void filter_stage_network(TwoPortNetwork *network, double angular_frequency, SCM stage) {
    scm_assert_foreign_object_type(filter_stage_type, stage);

    SCM type = scm_foreign_object_ref(stage, 0);

    SCM load = scm_foreign_object_ref(stage, 1);
    scm_assert_foreign_object_type(load_type, load);

    double complex impedance = load_impedance(angular_frequency, load);

    if (scm_eq_p(type, scm_from_utf8_symbol("series"))) {
        series_connected_network(network, impedance);
    }
    else if (scm_eq_p(type, scm_from_utf8_symbol("shunt"))) {
        shunt_connected_network(network, impedance);
    }
    else {
        scm_error_scm(
            scm_from_utf8_string("invalid-stage-type"), 
            SCM_BOOL_F, 
            scm_from_utf8_string("Invalid filter stage type."),
            SCM_BOOL_F,
            SCM_BOOL_F
        );
    }
}

void filter_network(TwoPortNetwork *network, TwoPortNetwork *work_area, double angular_frequency, SCM stages) {
    identity_network(network);
    for (int i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(stages); i++) {
        SCM stage = SCM_SIMPLE_VECTOR_REF(stages, i);
        scm_assert_foreign_object_type(filter_stage_type, stage);
        filter_stage_network(work_area, angular_frequency, stage);
        cascade_network(network, network, work_area);
    }
}
