#ifndef FILTOPT_COMPONENT
#define FILTOPT_COMPONENT

#include <complex.h>
#include <stdlib.h>

#include "preferred_value.h"
#include "random.h"

typedef enum {
    RESISTOR,
    CAPACITOR,
    INDUCTOR
} ComponentType;

typedef struct {
    ComponentType type;
    PreferredValue value;
    PreferredValue lower_limit;
    PreferredValue upper_limit;
    bool is_connected;
} Component;

Component new_component(
    ComponentType type, 
    PreferredValue value, 
    PreferredValue lower_limit, 
    PreferredValue upper_limit, 
    bool is_connected
);

bool components_equal(Component component1, Component component2);
double complex component_impedance(double angular_frequency, Component component);
void copy_component(Component source, Component *destination);
void component_random_update(Component *component, MTRand *prng);

#endif
