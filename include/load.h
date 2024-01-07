#ifndef FILTOPT_LOAD
#define FILTOPT_LOAD

#include <complex.h>
#include "component.h"
#include "random.h"
#include "memory.h"

typedef enum {
    COMPONENT_LOAD,
    SERIES_LOAD,
    PARALLEL_LOAD
} LoadType;

typedef struct Load {
    LoadType type;
    union {Load **loads; Component *component} element;
    size_t num_elements;
    void (*deallocate)(void *);
} Load;


double complex load_impedance(double angular_frequency, Load *load);
double complex admittance(double angular_frequency, Load *load);
Load *duplicate_load(Load *load, MemoryManager memory);
void copy_load(Load *source, Load *destination);
void free_load(Load *load);

void load_random_update(Load *load, MTRand *prng);
Load *new_component_load(Component *component, MemoryManager memory);
Load *new_series_load(Load *loads[], size_t num_loads, MemoryManager memory);
Load *new_parallel_load(Load *loads[], size_t num_loads, MemoryManager memory);
Load *make_component_load(const Component *component, MemoryManager memory);
Load *make_series_load(const Load *loads[], size_t num_loads, MemoryManager memory);
Load *make_parallel_load(const Load *loads[], size_t num_loads, MemoryManager memory);
#endif
