#ifndef FILTOPT_COMPONENT
#define FILTOPT_COMPONENT

#include <complex.h>
#include <stdlib.h>

#include "preferred_value.h"

typedef enum {
    RESISTOR,
    CAPACITOR,
    INDUCTOR
} ComponentType;

typedef struct {
    ComponentType type;
    PreferredValue *value;
    PreferredValue *lower_limit;
    PreferredValue *upper_limit;
    bool is_connected;
} Component;

Component *make_component(
    ComponentType type, 
    PreferredValue *value, 
    PreferredValue *lower_limit, 
    PreferredValue *upper_limit, 
    bool is_connected,
    void *allocate(size_t)
);
double complex component_impedance(double angular_frequency, Component *component);
void copy_component(Component *source, Component *destination);
Component *duplicate_component(Component *component, void *allocate(size_t), void deallocate(void *));
void free_component(Component *component, void deallocate(void *));
void component_random_update(Component *component, MTRand *prng);

#endif
