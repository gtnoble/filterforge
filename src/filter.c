#include <assert.h>
#include <stdlib.h>

#include "load.h"
#include "two_port_network.h"
#include "filter.h"

FilterStage *make_filter_stage(Load *load, FilterType type, void *allocate(size_t)) {
    FilterStage *stage = allocate(sizeof(FilterStage));
    stage->type = type;
    stage->load = load;
    return stage;
}

FilterStage *duplicate_filter_stage(FilterStage *stage, void *allocate(size_t), void deallocate(void *)) {
    return make_filter_stage(duplicate_load(stage->load, allocate, deallocate), stage->type, allocate);
}

void copy_filter_stage(FilterStage *source, FilterStage *destination) {
    destination->type = source->type;
    copy_load(source->load, destination->load);
}

void free_filter_stage(FilterStage *stage, void deallocate(void *)) {
    if (stage == NULL)
        return;
    free_load(stage->load, deallocate);
    deallocate(stage);
}

void filter_stage_random_update(FilterStage *stage, MTRand *prng) {
    load_random_update(stage->load, prng);
}

Filter *make_filter(
    FilterStage *stages[], 
    size_t num_stages, 
    void *allocate(size_t)
) {
    Filter *filter = allocate(sizeof(filter));
    if (filter == NULL) {
        return NULL;
    }

    filter->stages = stages;
    filter->num_stages = num_stages;
    return filter;
}

Filter *duplicate_filter(
    Filter *filter,
    void *allocate(size_t), 
    void deallocate(void *)
) {
    FilterStage *duplicated_stages[] = allocate(sizeof(FilterStage) * filter->num_stages);
    if (duplicated_stages == NULL)
        return NULL;
    for (size_t i = 0; i < filter->num_stages; i++) {
        duplicated_stages[i] = duplicate_filter_stage(filter->stages[i], allocate, deallocate);
        if (duplicated_stages[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free_filter_stage(duplicated_stages[i], deallocate);
            }
            deallocate(duplicated_stages);
        }
    }
    return make_filter(duplicated_stages, filter->num_stages, allocate);
}

void copy_filter(Filter *source, Filter *destination) {
    for (size_t i = 0; i < source->num_stages; i++) {
        copy_filter_stage(source->stages[i], destination->stages[i]);
    }
}

void free_filter(Filter *filter, void deallocate(void *)) {
    if (filter == NULL) {
        return;
    }
    for (size_t i = 0; i < filter->num_stages; i++) {
        free_filter_stage(filter->stages[i], deallocate);
    }
    deallocate(filter);
}

void filter_random_update(Filter *filter, MTRand *prng) {
    for (size_t i = 0; i < filter->num_stages; i++) {
        filter_stage_random_update(filter->stages[i], prng);
    }
}

void filter_stage_network(TwoPortNetwork *network, double angular_frequency, FilterStage *stage) {

    double complex impedance = load_impedance(angular_frequency, stage->load);

    switch (stage->type) {
        case SERIES_FILTER:
            series_connected_network(network, impedance);
            break;
        case SHUNT_FILTER:
            shunt_connected_network(network, impedance);
            break;
        default:
            assert(false);
    }
}

void get_filter_network(TwoPortNetwork *network, double angular_frequency, Filter *filter) {
    identity_network(network);
    TwoPortNetwork work_area;
    for (size_t i = 0; i < filter->num_stages; i++) {
        filter_stage_network(&work_area, angular_frequency, filter->stages[i]);
        cascade_network(network, network, &work_area);
    }
}

double complex filter_voltage_gain(double angular_frequency, Filter *filter) {
    TwoPortNetwork filter_network;
    get_filter_network(&filter_network, angular_frequency, filter);
    double complex complex_gain = network_voltage_gain(&filter_network);
    return complex_gain;
}
