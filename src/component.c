#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <complex.h>

#include "component.h"
#include "preferred_value.h"
#include "random.h"
#include "mtwister.h"

Component *make_component(
    ComponentType type, 
    PreferredValue *value, 
    PreferredValue *lower_limit, 
    PreferredValue *upper_limit, 
    bool is_connected,
    void *allocate(size_t)
);

Component *make_component(
    ComponentType type, 
    PreferredValue *value, 
    PreferredValue *lower_limit, 
    PreferredValue *upper_limit, 
    bool is_connected,
    void *allocate(size_t)
) {

    Component *component = allocate(sizeof(Component));
    component->type = type;
    component->value = value;
    component->lower_limit = lower_limit;
    component->upper_limit = upper_limit;
    component->is_connected = is_connected;
    return component;
}

void free_component(Component *component, void deallocate(void *)) {
    if (component == NULL)
        return;
    free_preferred_value(component->upper_limit, deallocate);
    free_preferred_value(component->lower_limit, deallocate);
    free_preferred_value(component->value, deallocate);
    deallocate(component);
}

Component *duplicate_component(Component *component, void *allocate(size_t), void deallocate(void *)) {
    PreferredValue *value = duplicate_preferred_value(component->value, allocate);
    if (value == NULL) {
        goto value_alloc_failure;
    }
    PreferredValue *lower_limit = duplicate_preferred_value(
        component->lower_limit,
        allocate
    );
    if (lower_limit == NULL) {
        goto lower_limit_alloc_failure;
    }

    PreferredValue *upper_limit = duplicate_preferred_value(
        component->upper_limit,
        allocate
    );
    if (upper_limit == NULL) {
        goto upper_limit_alloc_failure;
    }


    return make_component(
        component->type,
        value,
        lower_limit,
        upper_limit,
        component->is_connected,
        allocate
    );

    upper_limit_alloc_failure:
        free_preferred_value(lower_limit, deallocate);
    lower_limit_alloc_failure:
        free_preferred_value(value, deallocate);
    value_alloc_failure:
        return NULL;
}

void copy_component(Component *source, Component *destination) {
    copy_preferred_value(source->lower_limit, destination->lower_limit);
    copy_preferred_value(source->upper_limit, destination->upper_limit);
    copy_preferred_value(source->value, destination->value);

    destination->type = source->type;
    destination->is_connected = source->is_connected;
}


double complex component_impedance(double angular_frequency, Component *component) {
    assert(angular_frequency >= 0);

    if(! component->is_connected) {
        return INFINITY;
    }

    double value = evaluate_preferred_value(component->value);

    double complex impedance;
    switch (component->type) {
        case RESISTOR:
            impedance = value;
            break;
        case CAPACITOR:
            impedance = 1.0 / (I * angular_frequency * value);
            break;
        case INDUCTOR:
            impedance = I * angular_frequency * value;
            break;
        default:
            assert(false);
    }
    return impedance;
}

void component_random_update(Component *component, MTRand *prng) {

    component->is_connected = gen_random_bool(prng);

    if (component_values_equal(component->value, component->lower_limit)) {
        increment_component_value(component->value);
    }
    else if (component_values_equal(component->value, component->upper_limit)) {
        decrement_component_value(component->value);
    }
    else {
        if(gen_random_bool(prng)) {
            decrement_component_value(component->value);
        } 
        else {
            increment_component_value(component->value);
        }
    }
}





