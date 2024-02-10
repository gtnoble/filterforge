
#ifndef FILTERFORGE_CONFIG_INPUT
#define FILTERFORGE_CONFIG_INPUT

#include <jansson.h>

#include "filter.h"

Filter *load_filter(const char filename[]);

FilterStage *stage_from_config(json_t *stage_config);
Filter *filter_from_config(json_t *filter_config);
Load *load_from_config(json_t *load_config);
Component component_from_config(json_t *component_config);
Measurement measurement_from_config(json_t *measurement_config);

#endif