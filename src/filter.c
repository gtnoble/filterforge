#include <assert.h>
#include <stdlib.h>

#include "memory.h"
#include "load.h"
#include "two_port_network.h"
#include "filter.h"

FilterStage *new_filter_stage(Load *load, FilterStageType type, MemoryManager memory) {
    FilterStage *stage = memory.allocate(sizeof(FilterStage));
    if (stage == NULL) {
        return NULL;
    }

    stage->load = load;
    stage->type = type;
    stage->deallocate = memory.deallocate;

    return stage;
}

FilterStage *make_filter_stage(
    const Load *load, 
    FilterStageType type, 
    MemoryManager memory
) {
    Load *duplicated_load = duplicate_load(load, memory);
    if (duplicated_load == NULL) {
        goto load_duplication_failure;
    }

    FilterStage *stage = new_filter_stage(duplicated_load, type, memory);
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
    const FilterStage *stage, MemoryManager memory) {
    return make_filter_stage(stage->load, stage->type, memory);
}

void copy_filter_stage(FilterStage *source, FilterStage *destination) {
    destination->type = source->type;
    copy_load(source->load, destination->load);
}

void free_filter_stage(FilterStage *stage) {
    if (stage == NULL)
        return;
    free_load(stage->load);
    stage->deallocate(stage);
}

void filter_stage_random_update(FilterStage *stage, MTRand *prng) {
    load_random_update(stage->load, prng);
}

Filter *new_filter(
    FilterStage *stages[], 
    size_t num_stages, 
    MemoryManager memory
) {
    Filter *filter = memory.allocate(sizeof(filter));
    if (filter == NULL) {
        return NULL;
    }
    filter->stages = stages;
    filter->num_stages = num_stages;
    filter->deallocate = memory.deallocate;
    
    return filter;
}

Filter *make_filter(
    const FilterStage *stages[], 
    size_t num_stages, 
    MemoryManager memory
) {
    FilterStage *duplicated_stages[] = memory.allocate(sizeof(FilterStage *));
    if (duplicated_stages == NULL) {
        goto stages_alloc_failure;
    }

    size_t last_allocated_stage_index;
    for (size_t i = 0; i < num_stages; i++) {
        FilterStage *duplicated_stage = 
            duplicate_filter_stage(stages[i], memory);
        if (duplicated_stage == NULL) {
            last_allocated_stage_index = i - 1;
            goto stage_alloc_failure;
        }
        duplicated_stages[i] = duplicated_stage;
    }

    Filter *filter = new_filter(duplicated_stages, num_stages, memory);
    if (filter == NULL) {
        goto filter_alloc_failure;
    }

    return filter;

    filter_alloc_failure:
        for (size_t i = 0; i <= last_allocated_stage_index; i++) {
            free_filter_stage(duplicated_stages[i]);
        }
    stage_alloc_failure:
        memory.deallocate(duplicated_stages);
    stages_alloc_failure:
        return NULL;
}

Filter *duplicate_filter(
    const Filter *filter,
    MemoryManager memory
) {
    return make_filter(filter->stages, filter->num_stages, memory);
}

void copy_filter(Filter *source, Filter *destination) {
    for (size_t i = 0; i < source->num_stages; i++) {
        copy_filter_stage(source->stages[i], destination->stages[i]);
    }
}

void free_filter(Filter *filter) {
    if (filter == NULL) {
        return;
    }
    for (size_t i = 0; i < filter->num_stages; i++) {
        free_filter_stage(filter->stages[i]);
    }
    filter->deallocate(filter);
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
