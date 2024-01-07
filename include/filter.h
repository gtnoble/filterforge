#ifndef FILTOPT_FILTER
#define FILTOPT_FILTER

#include "load.h"
#include "two_port_network.h"
#include "memory.h"

typedef enum {
    SERIES_FILTER,
    SHUNT_FILTER
} FilterStageType;
typedef struct {
    FilterStageType type;
    Load *load;
    void (*deallocate)(void *);
} FilterStage;

typedef struct {
    FilterStage **stages;
    size_t num_stages;
    void (*deallocate)(void *);
} Filter;


FilterStage *new_filter_stage(Load *load, FilterStageType type, MemoryManager memory);
FilterStage *make_filter_stage(
    const Load *load, 
    FilterStageType type, 
    MemoryManager memory
);

Filter *new_filter(
    FilterStage *stages[], 
    size_t num_stages, 
    MemoryManager memory
);
Filter *make_filter(
    const FilterStage *stages[], 
    size_t num_stages, 
    MemoryManager memory
);

#endif
