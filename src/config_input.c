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
        handle_error(&error);
    }
    
    if(! fclose(config_file)) {
        fprintf(stderr, "Failed to close file: %s", filename);
        exit(1);
    }

    return config_root;
}

Filter *filter_from_config(json_t *filter_config) {
    if (! json_is_array(filter_config)) {
        fprintf(stderr, 
            "error: The root of the configuraion file is not an array."
            "The root should be an array of filter stages."
        );
        exit(1);
    }

    size_t num_stages = json_array_size(filter_config);
    FilterStage *stages[] = malloc(num_stages * sizeof(FilterStage *));

    for (size_t i = 0; i < num_stages; i++) {
        json_t *stage_config = json_array_get(filter_config, i);
        stages[i] = stage_from_config(stage_config);
    }

    return new_filter(stages, num_stages);
}

FilterStage *stage_from_config(json_t *stage_config) {

    json_t *load_config;
    char *stage_type_str;
    json_error_t error;
    if (
        json_unpack_ex(
            stage_config, 
            &error, 
            0, 
            "{s:s, s:o}", "type", stage_type_str, "load", load_config
        ) == -1
    ) {
        handle_error(&error);
    }

    Load *load = load_from_config(load_config);
    if (load == NULL) {
        return NULL;
    }

    FilterStageType stage_type = 
        filter_stage_type_string_to_code(stage_type_str);

    return new_filter_stage(load, stage_type);
}

Load *load_from_config(json_t *load_config) {

    json_t *element;
    char *load_type_str;
    json_error_t error;
    if (json_unpack_ex(
            load_config, 
            &error, 
            0, 
            "{s:s, s:o}", "type", load_type_str, "element", element
        )  == -1
    ) {
        handle_error(&error);
    }

    Load *load;
    if (! strcmp(load_type_str, "parallel"))
        load = new_parallel_load(
            loads_from_config_array(element), 
            json_array_size(element)
        );
    else if (! strcmp(load_type_str, "series"))
        load = new_series_load(
            loads_from_config_array(element), 
            json_array_size(element)
        );
    else if (! strcmp(load_type_str, "component")) {
        load = new_component_load(component_from_config(element));
    }
    else {
        fprintf(stderr, "error: %s is not a valid load type.", load_type_str);
        exit(1);
    }

    return load;
}

Load **loads_from_config_array(json_t *load_configs_array) {
    
    if (! json_is_array(load_configs_array)) {
        fprintf(stderr, "error: compound load elements must be an array");
        exit(1);
    }

    size_t num_loads = json_array_size(load_configs_array);
    Load *loads[] = malloc(sizeof(Load *));
    for (size_t i = 0; i < num_loads; i++) {
        loads[i] = load_from_config(json_array_get(load_configs_array, i));
    }

    return loads;
}

Component component_from_config(json_t *component_config) {

    double component_lower_limit_num;
    double component_upper_limit_num;
    double component_value_num;
    char *component_type_str;
    bool is_connected = true;
    json_error_t error;
    if (json_unpack_ex(
        component_config, 
        &error, 0, "{s:F, s:F, s:F, s:s, s?b}", 
        "lowerLimit", &component_lower_limit_num,
        "upperLimit", &component_upper_limit_num,
        "value", &component_value_num,
        "type", component_type_str,
        "isConnected", &is_connected
    ) == -1) {
        handle_error(&error);
    }

    PreferredValue component_lower_limit = 
        floor_preferred_value(component_lower_limit_num);
    PreferredValue component_upper_limit = 
        ceiling_preferred_value(component_upper_limit_num);
    PreferredValue component_value = 
        nearest_preferred_value(component_value_num);
    ComponentType component_type = 
        component_type_string_to_code(component_type_str);

    return new_component(
        component_type, 
        component_value, 
        component_lower_limit, 
        component_upper_limit, 
        is_connected
    );
}


ComponentType component_type_string_to_code(const char component_type[]) {
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
        exit(EXIT_FAILURE);
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
        extit(EXIT_FAILURE);
    }
    return stage_type_code;
}

void handle_error(json_error_t *error) {
    fprintf(
        stderr, 
        "error: Failed to parse JSON file - %s: "
        "Source - %s: "
        "Line number %d: "
        "Column %d: "
        "Position %d",
        error->text, error->source, error->line, error->column, error->position
    );
    exit(1);
}