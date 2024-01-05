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

typedef struct Load {
    LoadType type;
    union {Load **loads; Component *component} element;
    size_t num_elements;
} Load;


double complex load_impedance(double angular_frequency, Load *load);
double complex admittance(double angular_frequency, Load *load);
Load *duplicate_load(Load *load, void *allocate(size_t), void deallocate(void *));
void copy_load(Load *source, Load *destination);
void free_load(Load *load, void deallocate(void *));

void load_random_update(Load *load, MTRand *prng);
Load *make_component_load(Component *component, void *allocate(size_t));
Load *make_series_load(Load *loads[], void *allocate(size_t));
Load *make_parallel_load(Load *loads[], void *allocate(size_t));
#endif
