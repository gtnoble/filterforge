#include <complex.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "load.h"
#include "component.h"
#include "random.h"
#include "memory.h"

Load *new_compound_load(
    Load *loads[], 
    size_t num_loads,
    LoadType type, 
    MemoryManager memory
);

Load *new_component_load(
    Component *component, 
    MemoryManager memory
) {
    Load *load = memory.allocate(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    load->type = COMPONENT_LOAD;
    load->num_elements = 1;
    load->element.component = component;
    load->deallocate = memory.deallocate;
    return load;
}

Load *make_component_load(
    const Component *component, 
    MemoryManager memory
) {
    Component *duplicated_component = duplicate_component(component, memory);
    if (duplicate_component == NULL) {
        goto component_duplication_failure;
    }

    Load *load = new_component_load(duplicated_component, memory);
    if (load == NULL) {
        goto load_alloc_failure;
    }

    return load;

    load_alloc_failure:
        free_component(duplicate_component);
    component_duplication_failure:
        return NULL;
}

Load *new_compound_load(
    Load *loads[], 
    size_t num_loads,
    LoadType type, 
    MemoryManager memory
) {
    Load *load = memory.allocate(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }

    load->element.loads = loads;
    load->num_elements = num_loads;
    load->type = type;
    load->deallocate = memory.deallocate;

    return load;
}

Load *make_compound_load(
    const Load *loads[], 
    size_t num_loads,
    LoadType type, 
    MemoryManager memory
) {
    Load *duplicated_loads[] = memory.allocate(sizeof(Load *) * num_loads);
    if (duplicated_loads == NULL) {
        goto duplicated_loads_alloc_failure;
    }

    size_t last_successfully_duplicated_load;
    for (size_t i = 0; i < num_loads; i++) {
        Load *duplicated_load = duplicate_load(loads[i], memory);
        if (duplicate_load == NULL) {
            last_successfully_duplicated_load = i - 1;
            goto load_duplication_failure;
        }
        duplicated_loads[i] = duplicated_load;
    }

    Load *load = new_compound_load(duplicated_loads, num_loads, type, memory);
    if (load == NULL) {
        goto load_alloc_failure;
    }

    return load;
        
    load_alloc_failure:
        for (size_t i = 0; i <= last_successfully_duplicated_load; i++) {
            free_load(duplicated_loads[i]);
        }
    load_duplication_failure:
        memory.deallocate(duplicated_loads);
    duplicated_loads_alloc_failure:
        return NULL;
}

Load *new_series_load(Load *loads[], size_t num_loads, MemoryManager memory) {
    return new_compound_load(loads, num_loads, SERIES_LOAD, memory);
}

Load *make_series_load(const Load *loads[], size_t num_loads, MemoryManager memory) {
    return make_compound_load(loads, num_loads, SERIES_LOAD, memory);
}

Load *new_parallel_load(Load *loads[], size_t num_loads, MemoryManager memory) {
    return new_compound_load(loads, num_loads, PARALLEL_LOAD, memory);
}
Load *make_parallel_load(const Load *loads[], size_t num_loads, MemoryManager memory) {
    return make_compound_load(loads, num_loads, PARALLEL_LOAD, memory);
}

void free_load(Load *load) {
    if (load != NULL)
        return; 
    switch (load->type) {
        case COMPONENT_LOAD:
            free_component(load->element.component);
            load->deallocate(load);
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            for (size_t i = 0; i < load->num_elements; i++) {
                free_load(load->element.loads[i]);
            }
            load->deallocate(load);
        default:
            assert(false);
    }
}

void load_random_update(Load *load, MTRand *prng) {

    switch (get_load_type(load)) {
        case COMPONENT_LOAD:
            component_random_update(load->element.component, prng);
            break;
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            for (size_t i = 0; i < load->num_elements; i++) {
                load_random_update(load->element.loads[i], prng);
            }
            break;
        default:
            assert(false);
    }
}

Load *duplicate_load(Load *load, MemoryManager memory) {
    LoadType type = get_load_type(load);

    Load *duplicated_load;
    switch (load->type) {
        case COMPONENT_LOAD:
            duplicated_load = make_component_load(load->element.component, memory);
        case PARALLEL_LOAD:
        case SERIES_LOAD:
            duplicated_load = make_compound_load(
                load->element.loads, 
                load->num_elements, 
                load->type, 
                memory
            );
        break;
        default:
            assert(false);
    }
    return duplicated_load;
}

void copy_load(Load *source, Load *destination) {
    destination->type = source->type;
    switch (source->type) {
        case COMPONENT_LOAD:
            copy_component(source->element.component, destination->element.component);
            break;
        case PARALLEL_LOAD:
        case SERIES_LOAD:
            for (size_t i = 0; i < source->num_elements; i++) {
                copy_load(source->element.loads[i], destination->element.loads[i]);
            }
            break;
        default:
            assert(false);
    }
}

double complex load_impedance(double angular_frequency, Load *load) {
    assert(angular_frequency >= 0);

    double complex impedance;
    switch(load->type) {
        case COMPONENT_LOAD:
            impedance = component_impedance(angular_frequency, load->element.component);
            break;
        case SERIES_LOAD: {
            double complex sum_impedance = 0;
            for (size_t i = 0; i < load->num_elements; i++) {
                sum_impedance += load_impedance(angular_frequency, load->element.loads[i]);
            }
            impedance = sum_impedance;
        } break;
        case PARALLEL_LOAD: {
            double complex intermediate_impedance = 0;
            for (size_t i = 0; i < load->num_elements; i++) {
                intermediate_impedance += 1.0 / load_impedance(angular_frequency, load->element.loads[i]);
            }
            impedance = 1.0 / intermediate_impedance;
        } break;
        default:
            assert(false);
    }

    return impedance;
}

double complex admittance(double angular_frequency, Load *load) {
    assert(angular_frequency >= 0);

    return 1.0 / load_impedance(angular_frequency, load);
}

