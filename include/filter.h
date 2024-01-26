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
FilterStage *duplicate_filter_stage(const FilterStage *stage);
void copy_filter_stage(const FilterStage *source, FilterStage *destination);
bool filter_stages_equal(const FilterStage *stage1, const FilterStage *stage2);
void free_filter_stage(FilterStage *stage);
void free_filter_stage_node(FilterStage *stage);
void filter_stage_random_update(FilterStage *stage, MTRand *prng);

Filter *new_filter(FilterStage *stages[], size_t num_stages);
bool filters_equal(const Filter *filter1, const Filter *filter2);
Filter *duplicate_filter(const Filter *filter);
void copy_filter(const Filter *source, Filter *destination);
void free_filter_node(Filter *filter);
void free_filter(Filter *filter);
void filter_random_update(Filter *filter, MTRand *prng);
TwoPortNetwork filter_stage_network(double angular_frequency, const FilterStage *stage);
TwoPortNetwork get_filter_network(double angular_frequency, const Filter *filter);
double complex filter_voltage_gain(double angular_frequency, const Filter *filter);

#endif
