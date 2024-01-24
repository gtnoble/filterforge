#ifndef FILTOPT_LOAD
#define FILTOPT_LOAD

#include <complex.h>
#include "component.h"
#include "random.h"

typedef enum {
    COMPONENT_LOAD,
    SERIES_LOAD,
    PARALLEL_LOAD
} LoadType;

struct Load;
typedef struct Load Load;
typedef struct Load {
    LoadType type;
    union {Load **loads; Component component;} element;
    size_t num_elements;
} Load;


double complex load_impedance(double angular_frequency, Load *load);
Load *duplicate_load(Load *load);
bool loads_equal(Load *load1, Load *load2);
void copy_load(Load *source, Load *destination);
void free_load_node(Load *load);
void free_load(Load *load);

void load_random_update(Load *load, MTRand *prng);
Load *new_component_load(Component component);
Load *new_compound_load(Load **loads, size_t num_loads, LoadType type);
#endif
