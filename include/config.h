#ifndef FILTERFORGE_CONFIG
#define FILTERFORGE_CONFIG

/* Component JSON symbols */

static const char *k_component_lower_limit_key = "lowerLimit";
static const char *k_component_upper_limit_key = "upperLimit";
static const char *k_component_value_key = "value";
static const char *k_component_type_key = "type";
static const char *k_component_is_connected_key = "isConnected";

static const char *k_component_type_resistor_value = "resistor";
static const char *k_component_type_inductor_value = "inductor";
static const char *k_component_type_capacitor_value = "capacitor";

/* Load JSON symbols */

static const char *k_load_type_key = "type";
static const char *k_load_element_key = "element";

static const char *k_load_type_component_value = "component";
static const char *k_load_type_parallel_value = "parallel";
static const char *k_load_type_series_value = "series";

/* Filter stage JSON symbols */

static const char *k_filter_stage_type_key = "type";
static const char *k_filter_stage_load_key = "load";

static const char *k_filter_stage_type_series_value = "series";
static const char *k_filter_stage_type_shunt_value = "shunt";

#endif