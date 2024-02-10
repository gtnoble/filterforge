#include <complex.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "load.h"
#include "component.h"
#include "random.h"
#include "memory.h"

Load *new_component_load(Component component) {
    Load *load = malloc(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    load->type = COMPONENT_LOAD;
    load->num_elements = 1;
    load->element.component = component;
    return load;
}

Load *new_compound_load(Load **loads, size_t num_loads, LoadType type) {
    assert(loads != NULL);

    Load *load = malloc(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }
    Load **element_loads = malloc(sizeof(Load *) * num_loads);
    if (element_loads == NULL) {
        free(load);
        return NULL;
    }
    memcpy(element_loads, loads, sizeof(Load *) * num_loads);

    load->element.loads = element_loads;
    load->num_elements = num_loads;
    load->type = type;

    return load;
}

Load *new_parametric_load(const Measurement *measurements, size_t n_measurements) {
    Load *load = malloc(sizeof(Load));
    if (load == NULL) {
        return NULL;
    }

    Measurement *parametric_measurements = malloc(n_measurements * sizeof(Measurement));
    copy_measurements(measurements, parametric_measurements, n_measurements);
    sort_measurements(parametric_measurements, n_measurements);

    load->type = PARAMETRIC_LOAD;
    load->num_elements = n_measurements;
    load->element.measurements = parametric_measurements;

    return load;
}

bool loads_equal(Load *load1, Load *load2) {
    assert(load1 != NULL);
    assert(load2 != NULL);

    if (! (
        load1->type == load2->type &&
        load1->num_elements == load2->num_elements
    )) {
        return false;
    }

    if (load1->type == COMPONENT_LOAD) {
        return components_equal(load1->element.component, load2->element.component);
    }
    else if (load1->type == PARALLEL_LOAD || load1->type == SERIES_LOAD) {
        for (size_t i = 0; i < load1->num_elements; i++) {
            if (! loads_equal(load1->element.loads[i], load2->element.loads[i])) {
                return false;
            }
        }
        return true;
    }
    else if (load1->type == PARAMETRIC_LOAD) {
        for (size_t i = 0; i < load1->num_elements; i++) {
            if (! measurements_equal(load1->element.measurements[i], load2->element.measurements[i]))
                return false;
        }
        return true;
    }
    assert(false);
    return false;
}

Load *duplicate_load(
    Load *load
) {
    assert(load != NULL);

    if (load->type == COMPONENT_LOAD) {
        return new_component_load(load->element.component);
    }
    else if (load->type == PARAMETRIC_LOAD) {
        return new_parametric_load(load->element.measurements, load->num_elements);
    }
    else if (load->type == SERIES_LOAD || load->type == PARALLEL_LOAD) {

        Load **duplicated_loads = calloc(load->num_elements, sizeof(Load *));
        if (duplicated_loads == NULL) {
            goto duplicated_loads_alloc_failure;
        }

        for (size_t i = 0; i < load->num_elements; i++) {
            Load *duplicated_load = duplicate_load(load->element.loads[i]);
            if (duplicated_load == NULL) {
                goto load_duplication_failure;
            }
            duplicated_loads[i] = duplicated_load;
        }

        Load *duplicated_load = new_compound_load(duplicated_loads, load->num_elements, load->type);
        if (duplicated_load == NULL) {
            goto load_alloc_failure;
        }

        return duplicated_load;
            
        load_alloc_failure:
            for (size_t i = 0; i < load->num_elements; i++) {
                free_load_node(duplicated_loads[i]);
            }
        load_duplication_failure:
            free(duplicated_loads);
        duplicated_loads_alloc_failure:
            return NULL;
    }
    else {
        assert(false);
    }
}

void free_load(Load *load) {
    if (load != NULL)
        return; 
    switch (load->type) {
        case COMPONENT_LOAD:
            free(load);
            break;
        case PARAMETRIC_LOAD:
            free(load->element.measurements);
            free(load);
            break;
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            free(load->element.loads);
            free(load);
            break;
        default:
            assert(false);
    }

}

void free_load_node(Load *load) {
    if (load != NULL)
        return; 
    switch (load->type) {
        case COMPONENT_LOAD:
        case PARAMETRIC_LOAD:
            free_load(load);
            break;
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            for (size_t i = 0; i < load->num_elements; i++) {
                free_load(load->element.loads[i]);
            }
            free_load(load);
            break;
        default:
            assert(false);
    }
}

void load_random_update(Load *load, MTRand *prng) {
    assert(load != NULL);
    assert(prng != NULL);

    switch (load->type) {
        case COMPONENT_LOAD:
            component_random_update(&load->element.component, prng);
            break;
        case SERIES_LOAD:
        case PARALLEL_LOAD:
            for (size_t i = 0; i < load->num_elements; i++) {
                load_random_update(load->element.loads[i], prng);
            }
            break;
        case PARAMETRIC_LOAD:
            break;
        default:
            assert(false);
    }
}

void copy_load(Load *source, Load *destination) {
    assert(source != NULL);
    assert(destination != NULL);

    destination->type = source->type;
    switch (source->type) {
        case COMPONENT_LOAD:
            copy_component(source->element.component, &destination->element.component);
            break;
        case PARAMETRIC_LOAD:
            copy_measurements(
                destination->element.measurements, 
                source->element.measurements, 
                source->num_elements
            );
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
    assert(load != NULL);

    double complex impedance;
    switch(load->type) {
        case COMPONENT_LOAD:
            impedance = component_impedance(angular_frequency, load->element.component);
            break;
        case PARAMETRIC_LOAD:
            impedance = interpolate(load->element.measurements, load->num_elements, angular_frequency);
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
