#ifndef FILTOPT_FILTER
#define FILTOPT_FILTER

#include "load.h"
#include "two_port_network.h"

typedef enum {
    SERIES_FILTER,
    SHUNT_FILTER
} FilterStageType;
typedef struct {
    FilterStageType type;
    Load *load;
} FilterStage;

typedef struct {
    FilterStage **stages;
    size_t num_stages;
} Filter;


FilterStage *new_filter_stage(Load *load, FilterStageType type);
FilterStage *make_filter_stage(
    const Load *load, 
    FilterStageType type
);

Filter *new_filter(
    FilterStage *stages[], 
    size_t num_stages
);
Filter *make_filter(
    const FilterStage *stages[], 
    size_t num_stages 
);

#endif
