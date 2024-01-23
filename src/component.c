#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <complex.h>

#include "component.h"
#include "preferred_value.h"
#include "random.h"
#include "mtwister.h"
#include "memory.h"

Component new_component(
    ComponentType type, 
    PreferredValue value, 
    PreferredValue lower_limit, 
    PreferredValue upper_limit, 
    bool is_connected
) {
    assert(preferred_values_greater_than_or_equal(value, lower_limit));
    assert(preferred_values_less_than_or_equal(value, upper_limit));

    return (Component) {
        .type = type,
        .value = value,
        .lower_limit = lower_limit,
        .upper_limit = upper_limit,
        .is_connected = is_connected
    };
}

bool components_equal(Component component1, Component component2) {
    return 
        component1.type == component2.type &&
        preferred_values_equal(component1.value, component2.value) &&
        preferred_values_equal(component1.lower_limit, component2.lower_limit) &&
        preferred_values_equal(component1.upper_limit, component2.upper_limit) &&
        component1.is_connected == component2.is_connected;
}

void copy_component(Component source, Component *destination) {
    destination->lower_limit = source.lower_limit;
    destination->upper_limit = source.upper_limit;
    destination->value = source.value;
    destination->type = source.type;
    destination->is_connected = source.is_connected;
}


double complex component_impedance(double angular_frequency, Component component) {
    assert(angular_frequency >= 0);

    if(! component.is_connected) {
        return INFINITY;
    }

    double value = evaluate_preferred_value(component.value);

    double complex impedance;
    switch (component.type) {
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

    if (preferred_values_equal(component->value, component->lower_limit)) {
        increment_preferred_value(&component->value);
    }
    else if (preferred_values_equal(component->value, component->upper_limit)) {
        decrement_preferred_value(&component->value);
    }
    else {
        if(gen_random_bool(prng)) {
            decrement_preferred_value(&component->value);
        } 
        else {
            increment_preferred_value(&component->value);
        }
    }
}





