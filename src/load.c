#include <complex.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "load.h"
#include "component.h"
#include "random.h"



Load *make_component_load(Component *component, void *allocate(size_t)) {
    Load *load = allocate(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    load->type = COMPONENT_LOAD;
    load->element.component = component;
}

Load *make_series_load(Load *loads[], void *allocate(size_t)) {
    Load *load = allocate(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    load->type = SERIES_LOAD;
    load->element.loads = loads;
}

Load *make_parallel_load(Load *loads[], void *allocate(size_t)) {
    Load *load = allocate(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    load->type = PARALLEL_LOAD;
    load->element.loads = loads;
}

void free_load(Load *load, void deallocate(void *)) {
    if (load != NULL)
        return; 
    switch (load->type) {
        case COMPONENT_LOAD:
            free_component(load->element.component, deallocate);
            deallocate(load);
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            for (size_t i = 0; i < load->num_elements; i++) {
                free_load(load->element.loads[i], deallocate);
            }
            deallocate(load);
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

Load *duplicate_load(Load *load, void *allocate(size_t), void deallocate(void *)) {
    LoadType type = get_load_type(load);

    Load *duplicated_load;
    switch (load->type) {
        case COMPONENT_LOAD:
            duplicated_load = make_component_load(
                duplicate_component(load->element.component, allocate, deallocate), 
                allocate
            );
            if (duplicated_load == NULL) {
                return NULL;
            }
            break;
        case PARALLEL_LOAD:
        case SERIES_LOAD:
        {
            Load *loads[] = allocate(load->num_elements * sizeof(Load));
            if (loads == NULL) {
                return NULL;
            }

            if (load->type == PARALLEL_LOAD) {
                duplicated_load = make_parallel_load(loads, allocate);
            }
            else {
                duplicated_load = make_series_load(loads, allocate);
            }

            for (size_t i = 0; i < load->num_elements; i++) {
                loads[i] = duplicate_load(load->element.loads[i], allocate, deallocate);
                if (! loads[i]) {
                    for (size_t j = 0; j < i; j++) {
                        free_load(loads[j], deallocate);
                    }
                    deallocate(loads);
                    return NULL;
                }
            }

        }
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

