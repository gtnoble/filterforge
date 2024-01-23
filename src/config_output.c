#include <jansson.h>
#include <string.h>

#include "component.h"


json_t *component_to_config(Component component) {
    json_error_t error;
    char component_type[20];
    component_type_code_to_string(component.type, component_type);
    return json_pack_ex(
        &error,
        0,
        "{s:f, s:f, s:f, s:s, s:b}",
        "lowerLimit", evaluate_preferred_value(component.lower_limit),
        "upperLimit", evaluate_preferred_value(component.upper_limit),
        "value", evaluate_preferred_value(component.value),
        "type", component_type,
        "isConnected", component.is_connected
    );
}


void component_type_code_to_string(ComponentType type_code, char type_str[]) {
    switch (type_code) {
        case RESISTOR:
            strcpy(type_str, "resistor");
            break;
        case CAPACITOR:
            strcpy(type_str, "capacitor");
            break;
        case INDUCTOR:
            strcpy(type_str, "inductor");
            break;
        default:
            assert(false);
    }
}