#include <assert.h>
#include <stdlib.h>

#include "load.h"
#include "two_port_network.h"
#include "filter.h"

SCM filter_stage_type;

SCM series_filter_symbol;
SCM shunt_filter_symbol;

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

    series_filter_symbol = scm_from_utf8_symbol("series");
    shunt_filter_symbol = scm_from_utf8_symbol("shunt");

    scm_c_define_gsubr("make-series-filter-stage", 1, 0, 0, make_series_filter_stage);
    scm_c_define_gsubr("make-shunt-filter-stage", 1, 0, 0, make_shunt_filter_stage);
}

SCM make_series_filter_stage(SCM load) {
    return scm_make_foreign_object_2(filter_stage_type, series_filter_symbol, load);
}

SCM make_shunt_filter_stage(SCM load) {
    return scm_make_foreign_object_2(filter_stage_type, shunt_filter_symbol, load);
}

void filter_stage_network(TwoPortNetwork *network, double angular_frequency, SCM stage) {
    scm_assert_foreign_object_type(filter_stage_type, stage);

    SCM type = scm_foreign_object_ref(stage, 0);

    SCM load = scm_foreign_object_ref(stage, 1);
    scm_assert_foreign_object_type(load_type, load);

    double complex impedance = load_impedance(angular_frequency, load);

    if (scm_eq_p(type, series_filter_symbol)) {
        series_connected_network(network, impedance);
    }
    else if (scm_eq_p(type, shunt_filter_symbol)) {
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

void get_filter_network(TwoPortNetwork *network, double angular_frequency, SCM stages) {
    identity_network(network);
    TwoPortNetwork work_area;
    for (int i = 0; i < SCM_SIMPLE_VECTOR_LENGTH(stages); i++) {
        SCM stage = SCM_SIMPLE_VECTOR_REF(stages, i);
        scm_assert_foreign_object_type(filter_stage_type, stage);
        filter_stage_network(&work_area, angular_frequency, stage);
        cascade_network(network, network, &work_area);
    }
}

double complex filter_voltage_gain(double angular_frequency, SCM stages) {
    TwoPortNetwork filter_network;
    get_filter_network(&filter_network, angular_frequency, stages);
    return network_voltage_gain(&filter_network);
}
