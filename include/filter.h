#ifndef FILTOPT_FILTER
#define FILTOPT_FILTER

#include "load.h"
#include "two_port_network.h"

typedef enum {
    SERIES_FILTER,
    SHUNT_FILTER
} FilterType;
typedef struct {
    FilterType type;
    Load *load;
} FilterStage;

typedef struct {
    FilterStage **stages;
    size_t num_stages;
} Filter;


FilterStage *make_filter_stage(Load *load, FilterType type, void *allocate(size_t)) {

#endif
