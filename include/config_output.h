#ifndef FILTERFORGE_OUTPUT
#define FILTERFORGE_OUTPUT

#include "filter.h"

void save_filter(const char filename[], Filter *filter);

json_t *component_to_config(Component component);
json_t *load_to_config(Load *load);
json_t *filter_stage_to_config(FilterStage *stage);
json_t *filter_to_config(Filter *filter);

#endif