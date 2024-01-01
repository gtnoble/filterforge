#include <libguile.h>
#include <stdbool.h>

extern SCM preferred_component_value_type;

void init_preferred_component_value_type(void);
double evaluated_component_value(SCM preferred_value);
SCM increment_component_value(SCM value); 
SCM decrement_component_value(SCM value);
bool component_values_equal(SCM value1, SCM value2);