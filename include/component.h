#ifndef FILTOPT_COMPONENT
#define FILTOPT_COMPONENT

#include <complex.h>
#include <stdlib.h>

#include "preferred_value.h"
#include "memory.h"

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
    void (*deallocate)(void *);
} Component;

Component *new_component(
    ComponentType type, 
    PreferredValue value, 
    PreferredValue lower_limit, 
    PreferredValue upper_limit, 
    bool is_connected,
    MemoryManager memory
);

Component *make_component(
    ComponentType type, 
    const PreferredValue value, 
    const PreferredValue lower_limit, 
    const PreferredValue upper_limit, 
    bool is_connected,
    MemoryManager memory
);
double complex component_impedance(double angular_frequency, Component *component);
void copy_component(Component *source, Component *destination);
Component *duplicate_component(const Component *component, MemoryManager memory);
void free_component(Component *component);
void component_random_update(Component *component, MTRand *prng);

#endif
