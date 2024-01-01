#include "component.h"
#include "filter.h"
#include "load.h"
#include "preferred_value.h"
#include "two_port_network.h"
#include <libguile.h>

void init_filtopt() {
    init_component_type();
    init_preferred_component_value_type();
    init_load_type();
    init_filter_stage_type();
}


