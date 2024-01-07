#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filter.h"
#include "component.h"
#include "load.h"

json_t *load_config_file(const char filename[]) {
    FILE *config_file = fopen(filename, "r");
    if (config_file == NULL) {
        fprintf(stderr, "error: Failed to open file: %s", filename);
        exit(1);
    }

    json_error_t error;
    json_t *config_root = json_loadf(config_file, 0, &error);

    if (config_root == NULL) {
        fprintf(
            stderr, 
            "error: Failed to parse JSON file - %s: "
            "Source - %s: "
            "Line number %d: "
            "Column %d: "
            "Position %d",
            error.text, error.source, error.line, error.column, error.position
        );
        exit(1);
    }
    
    if(! fclose(config_file)) {
        fprintf(stderr, "Failed to close file: %s", filename);
        exit(1);
    }

    return config_root;
}

Filter *filter_from_config(json_t *filter_config, MemoryManager memory) {
    if (! json_is_array(filter_config)) {
        fprintf(stderr, 
            "error: The root of the configuraion file is not an array."
            "The root should be an array of filter stages."
        );
        exit(1);
    }

    size_t num_stages = json_array_size(filter_config);
    FilterStage *stages[] = memory.allocate(num_stages * sizeof(FilterStage *));

    for (size_t i = 0; i < num_stages; i++) {
        json_t *stage_config = json_array_get(filter_config, i);
        stages[i] = stage_from_config(stage_config, memory);
    }

    return new_filter(stages, num_stages, memory);
}

FilterStage *stage_from_config(json_t *stage_config, MemoryManager memory) {
    if (! json_is_object(stage_config)) {
        fprintf(stderr, "error: filter stage configurations must be JSON objects");
        exit(1);
    }
    json_t *load_config = json_object_get(stage_config, "load");
    Load *load = load_from_config(load_config, memory);
    if (load == NULL) {
        return NULL;
    }

    FilterStageType stage_type = 
        filter_stage_type_string_to_code(json_property_string_get(stage_config, "type"));

    return new_filter_stage(load, stage_type, memory);
}

Load *load_from_config(json_t *load_config, MemoryManager memory) {
    if (! json_is_object(load_config)) {
        fprintf(stderr, "error: load configuration must be a JSON object");
        exit(1);
    }

    json_t *element = json_object_get(load_config, "element");
    if (! (json_is_object(element) || json_is_array(element))) {
        fprintf(stderr, "error: load element must either be a component or an array of loads");
        exit(1);
    }

    char load_type_str[] = json_property_string_get(load_config, "type");
    Load *load;
    if (! strcmp(load_type_str, "parallel"))
        load = new_parallel_load(loads_from_config_array(element, memory), json_array_size(element), memory);
    else if (! strcmp(load_type_str, "series"))
        load = new_series_load(loads_from_config_array(element, memory), json_array_size(element), memory);
    else if (! strcmp(load_type_str, "component")) {
        Component *component = component_from_config(element, memory);
        if (component == NULL)
            return NULL;
        load = new_component_load(component, memory);
    }
    else {
        fprintf(stderr, "error: %s is not a valid load type.", load_type_str);
        exit(1);
    }

    return load;
}

Load **loads_from_config_array(json_t *load_configs_array, MemoryManager memory) {
    
    if (! json_is_array(load_configs_array)) {
        fprintf(stderr, "error: compound load elements must be an array");
        exit(1);
    }

    size_t num_loads = json_array_size(load_configs_array);
    Load *loads[] = memory.allocate(sizeof(Load *));
    for (size_t i = 0; i < num_loads; i++) {
        loads[i] = load_from_config(json_array_get(load_configs_array, i), memory);
    }

    return loads;
}

Component *component_from_config(json_t *component_config, MemoryManager memory) {
    if (! json_is_object(component_config)) {
        fprintf(stderr, "error: component must be a json object");
        exit(1);
    }

    PreferredValue component_lower_limit = floor_preferred_value(json_property_number_get(component_config, "lowerLimit"));
    PreferredValue component_upper_limit = ceiling_preferred_value(json_property_number_get(component_config, "upperLimit"));
    PreferredValue component_value = nearest_preferred_value(json_property_number_get(component_config, "value"));
    ComponentType component_type = component_type_string_to_code(json_property_string_get(component_config, "type"));

    bool is_connected = true;
    json_t *component_connected_config = json_object_get(component_config, "isConnected");
    if (component_connected_config != NULL && ! json_is_boolean(component_connected_config)) {
        fprintf(stderr, "If isConnected is specified, it must be a boolean value");
        exit(1);
    }

    return new_component(component_type, component_value, component_lower_limit, component_upper_limit, is_connected, memory);
}

double json_property_number_get(const json_t *config, char key[]) {
    json_t *number_config = json_object_get(config, key);
    if (! json_is_number(number_config)) {
        fprintf(stderr, "error: %s must be a number", key);
        exit(1);
    }
    return json_number_value(number_config);
}

char *json_property_string_get(const json_t *config, char key[]) {
    json_t *component_type_config = json_object_get(config, key);
    if (! json_is_string(component_type_config)) {
        fprintf(stderr, "error: %s must be a string", key);
        exit(1);
    }
    return json_string_value(component_type_config);
}

ComponentType component_type_string_to_code(char component_type[]) {
    ComponentType component_type_code;
    if (! strcmp(component_type, "resistor")) {
        component_type_code = RESISTOR;
    }
    else if (! strcmp(component_type, "capacitor")) {
        component_type_code = CAPACITOR;
    }
    else if (! strcmp(component_type, "inductor")) {
        component_type_code = INDUCTOR;
    }
    else {
        fprintf(stderr, "error: %s is not a valid component type", component_type);
        exit(1);
    }
    return component_type_code;

}

FilterStageType filter_stage_type_string_to_code(char stage_type[]) {
    FilterStageType stage_type_code;
    if (! strcmp(stage_type, "series")) {
        stage_type_code = SERIES_FILTER;
    }
    else if (! strcmp(stage_type, "shunt")) {
        stage_type_code = SHUNT_FILTER;
    }
    else {
        fprintf(stderr, "error: %s is not a valid filter stage type", stage_type);
    }
    return stage_type_code;
}
