#include <assert.h>
#include <stdlib.h>

#include "memory.h"
#include "load.h"
#include "two_port_network.h"
#include "filter.h"

FilterStage *new_filter_stage(Load *load, FilterStageType type) {
    FilterStage *stage = malloc(sizeof(FilterStage));
    if (stage == NULL) {
        return NULL;
    }

    stage->load = load;
    stage->type = type;

    return stage;
}

FilterStage *make_filter_stage(
    const Load *load, 
    FilterStageType type 
) {
    Load *duplicated_load = duplicate_load(load);
    if (duplicated_load == NULL) {
        goto load_duplication_failure;
    }

    FilterStage *stage = new_filter_stage(duplicated_load, type);
    if (stage == NULL) {
        goto stage_alloc_failure;
    }

    return stage;

    stage_alloc_failure:
        free_load(duplicated_load);
    load_duplication_failure:
        return NULL;
}

FilterStage *duplicate_filter_stage(
    const FilterStage *stage) {
    return make_filter_stage(stage->load, stage->type);
}

void copy_filter_stage(FilterStage source, FilterStage *destination) {
    destination->type = source.type;
    copy_load(source.load, destination->load);
}

void free_filter_stage(FilterStage *stage) {
    if (stage == NULL)
        return;
    free_load(stage->load);
    free(stage);
}

void filter_stage_random_update(FilterStage *stage, MTRand *prng) {
    load_random_update(stage->load, prng);
}

Filter *new_filter(
    FilterStage *stages[], 
    size_t num_stages
) {
    Filter *filter = malloc(sizeof(filter));
    if (filter == NULL) {
        return NULL;
    }
    filter->stages = stages;
    filter->num_stages = num_stages;
    
    return filter;
}

Filter *make_filter(
    const FilterStage *stages[], 
    size_t num_stages
) {
    FilterStage *duplicated_stages[] = malloc(sizeof(FilterStage *));
    if (duplicated_stages == NULL) {
        goto stages_alloc_failure;
    }

    size_t last_allocated_stage_index;
    for (size_t i = 0; i < num_stages; i++) {
        FilterStage *duplicated_stage = 
            duplicate_filter_stage(stages[i]);
        if (duplicated_stage == NULL) {
            last_allocated_stage_index = i - 1;
            goto stage_alloc_failure;
        }
        duplicated_stages[i] = duplicated_stage;
    }

    Filter *filter = new_filter(duplicated_stages, num_stages);
    if (filter == NULL) {
        goto filter_alloc_failure;
    }

    return filter;

    filter_alloc_failure:
        for (size_t i = 0; i <= last_allocated_stage_index; i++) {
            free_filter_stage(duplicated_stages[i]);
        }
    stage_alloc_failure:
        free(duplicated_stages);
    stages_alloc_failure:
        return NULL;
}

Filter *duplicate_filter(
    const Filter *filter
) {
    return make_filter(filter->stages, filter->num_stages);
}

void copy_filter(Filter source, Filter *destination) {
    for (size_t i = 0; i < source.num_stages; i++) {
        copy_filter_stage(*source.stages[i], destination->stages[i]);
    }
}

void free_filter(Filter *filter) {
    if (filter == NULL) {
        return;
    }
    for (size_t i = 0; i < filter->num_stages; i++) {
        free_filter_stage(filter->stages[i]);
    }
    free(filter);
}

void filter_random_update(Filter *filter, MTRand *prng) {
    for (size_t i = 0; i < filter->num_stages; i++) {
        filter_stage_random_update(filter->stages[i], prng);
    }
}

TwoPortNetwork filter_stage_network(double angular_frequency, FilterStage *stage) {

    double complex impedance = load_impedance(angular_frequency, stage->load);

    switch (stage->type) {
        case SERIES_FILTER:
            return series_connected_network(impedance);
            break;
        case SHUNT_FILTER:
            return shunt_connected_network(impedance);
            break;
        default:
            assert(false);
            return;
    }
}

TwoPortNetwork get_filter_network(double angular_frequency, Filter filter) {
    TwoPortNetwork network = identity_network();
    for (size_t i = 0; i < filter.num_stages; i++) {
        network = cascade_network(
            network, 
            filter_stage_network(angular_frequency, filter.stages[i])
        );
    }
    return network;
}

double complex filter_voltage_gain(double angular_frequency, Filter filter) {
    return network_voltage_gain(
        get_filter_network(angular_frequency, filter)
    );
}
