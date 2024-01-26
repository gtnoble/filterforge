#include <assert.h>
#include <stdlib.h>

#include "load.h"
#include "two_port_network.h"
#include "filter.h"

FilterStage *new_filter_stage(Load *load, FilterStageType type) {
    assert(load != NULL);
    assert(type == SERIES_FILTER || type == SHUNT_FILTER);

    FilterStage *stage = malloc(sizeof(FilterStage));
    if (stage == NULL) {
        return NULL;
    }

    stage->load = load;
    stage->type = type;

    return stage;
}


FilterStage *duplicate_filter_stage(const FilterStage *stage) {
    assert(stage != NULL);

    Load *duplicated_load = duplicate_load(stage->load);
    if (duplicated_load == NULL) {
        goto load_duplication_failure;
    }

    FilterStage *duplicated_stage = new_filter_stage(duplicated_load, stage->type);
    if (duplicated_stage == NULL) {
        goto stage_alloc_failure;
    }

    return duplicated_stage;

    stage_alloc_failure:
        free_load_node(duplicated_load);
    load_duplication_failure:
        return NULL;
}

bool filter_stages_equal(const FilterStage *stage1, const FilterStage *stage2) {
    return
        stage1->type == stage2->type &&
        loads_equal(stage1->load, stage2->load);
}

void copy_filter_stage(const FilterStage *source, FilterStage *destination) {
    assert(destination != NULL);

    destination->type = source->type;
    copy_load(source->load, destination->load);
}

void free_filter_stage(FilterStage *stage) {
    free(stage);
}

void free_filter_stage_node(FilterStage *stage) {
    if (stage == NULL)
        return;
    free_load_node(stage->load);
    free_filter_stage(stage);
}

void filter_stage_random_update(FilterStage *stage, MTRand *prng) {
    assert(stage != NULL);
    assert(prng != NULL);

    load_random_update(stage->load, prng);
}

Filter *new_filter(FilterStage *stages[], size_t num_stages) {
    assert(stages != NULL);

    Filter *filter = malloc(sizeof(filter));
    if (filter == NULL) {
        return NULL;
    }

    FilterStage **stages_duplicate = malloc(sizeof(FilterStage *) * num_stages);
    if (stages_duplicate == NULL) {
        free_filter(filter);
        return NULL;
    }
    for (size_t i = 0; i < num_stages; i++) {
        stages_duplicate[i] = stages[i];
    }

    filter->stages = stages_duplicate;
    filter->num_stages = num_stages;
    
    return filter;
}

bool filters_equal(const Filter *filter1, const Filter *filter2) {
    if (filter1->num_stages != filter2->num_stages) {
        return false;
    }

    for (size_t i = 0; i < filter1->num_stages; i++) {
        if (! filter_stages_equal(filter1->stages[i], filter2->stages[i]))
            return false;
    }
    return true;
}

Filter *duplicate_filter(const Filter *filter) {
    assert(filter != NULL);

    FilterStage **temp_duplicated_stages = calloc(sizeof(FilterStage *), filter->num_stages);
    if (temp_duplicated_stages == NULL) {
        goto stages_alloc_failure;
    }

    for (size_t i = 0; i < filter->num_stages; i++) {
        FilterStage *duplicated_stage = 
            duplicate_filter_stage(filter->stages[i]);
        if (duplicated_stage == NULL) {
            goto stage_alloc_failure;
        }
        temp_duplicated_stages[i] = duplicated_stage;
    }

    Filter *duplicated_filter = new_filter(temp_duplicated_stages, filter->num_stages);
    free(temp_duplicated_stages);
    temp_duplicated_stages = NULL;

    if (duplicated_filter == NULL) {
        goto filter_alloc_failure;
    }

    return duplicated_filter;

    filter_alloc_failure:
        for (size_t i = 0; i < filter->num_stages; i++) {
            free_filter_stage_node(temp_duplicated_stages[i]);
        }
    stage_alloc_failure:
        free(temp_duplicated_stages);
    stages_alloc_failure:
        return NULL;
}

void copy_filter(const Filter *source, Filter *destination) {
    assert(destination != NULL);
    assert(source->num_stages == destination->num_stages);

    for (size_t i = 0; i < source->num_stages; i++) {
        copy_filter_stage(source->stages[i], destination->stages[i]);
    }
}

void free_filter_node(Filter *filter) {
    if (filter == NULL) {
        return;
    }
    for (size_t i = 0; i < filter->num_stages; i++) {
        free_filter_stage_node(filter->stages[i]);
    }
    free_filter(filter);
}

void free_filter(Filter *filter) {
    free(filter);
}

void filter_random_update(Filter *filter, MTRand *prng) {
    assert(filter != NULL);
    assert(prng != NULL);

    for (size_t i = 0; i < filter->num_stages; i++) {
        filter_stage_random_update(filter->stages[i], prng);
    }
}

TwoPortNetwork filter_stage_network(double angular_frequency, const FilterStage *stage) {
    assert(stage != NULL);
    assert(stage->type == SERIES_FILTER || stage->type == SHUNT_FILTER);

    double complex impedance = load_impedance(angular_frequency, stage->load);

    switch (stage->type) {
        case SERIES_FILTER:
            return series_connected_network(impedance);
            break;
        default:
            return shunt_connected_network(impedance);
            break;
    }
}

TwoPortNetwork get_filter_network(double angular_frequency, const Filter *filter) {
    TwoPortNetwork network = identity_network();
    for (size_t i = 0; i < filter->num_stages; i++) {
        network = cascade_network(
            network, 
            filter_stage_network(angular_frequency, filter->stages[i])
        );
    }
    return network;
}

double complex filter_voltage_gain(double angular_frequency, const Filter *filter) {
    return network_voltage_gain(
        get_filter_network(angular_frequency, filter)
    );
}
