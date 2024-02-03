#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "filter.h"
#include "component.h"
#include "load.h"
#include "config.h"
#include "config_input.h"

LoadType load_type_string_to_code(const char load_type[]);
ComponentType component_type_string_to_code(const char component_type[]);
FilterStageType filter_stage_type_string_to_code(char stage_type[]);
static void handle_error(json_error_t *error);

Filter *load_filter(const char filename[]) {

    json_error_t error;
    json_t *filter_config = json_load_file(filename, 0, &error);

    Filter *filter = filter_from_config(filter_config);

    json_decref(filter_config);

    return filter;
}

Filter *filter_from_config(json_t *filter_config) {
    if (! json_is_array(filter_config)) {
        fprintf(stderr, 
            "error: The root of the configuraion file is not an array."
            "The root should be an array of filter stages."
        );
        exit(EXIT_FAILURE);
    }

    size_t num_stages = json_array_size(filter_config);
    FilterStage **stages = malloc(num_stages * sizeof(FilterStage *));

    for (size_t i = 0; i < num_stages; i++) {
        json_t *stage_config = json_array_get(filter_config, i);
        if (! json_is_object(stage_config)) {
            fprintf(stderr,
                "error: Filter stage %lu is not an object.",
                i + 1
            );
            exit(EXIT_FAILURE);
        }
        stages[i] = stage_from_config(stage_config);
    }

    Filter *filter = new_filter(stages, num_stages);
    free(stages);

    return filter;
}

FilterStage *stage_from_config(json_t *stage_config) {

    json_t *load_config = NULL;
    char *stage_type_str = NULL;
    static const char *k_filter_stage_schema = "{s:s, s:o}";
    json_error_t error;
    if (
        json_unpack_ex(
            stage_config, 
            &error, 
            0, 
            k_filter_stage_schema, 
            k_filter_stage_type_key, &stage_type_str, 
            k_filter_stage_load_key, &load_config
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

    json_t *element = NULL;
    char *load_type_str = NULL;

    static const char *k_load_config_schema = "{s:s, s:o}";
    json_error_t error;
    if (json_unpack_ex(
            load_config, 
            &error, 
            0, 
            k_load_config_schema, 
            k_load_type_key, &load_type_str, 
            k_load_element_key, &element
        )  == -1
    ) {
        handle_error(&error);
    }

    Load *load;
    LoadType load_type = load_type_string_to_code(load_type_str);

    if (load_type == PARALLEL_LOAD || load_type == SERIES_LOAD) {
        if (! json_is_array(element)) {
            fprintf(stderr, "error: compound load elements must be an array");
            exit(EXIT_FAILURE);
        }

        size_t num_loads = json_array_size(element);
        Load **loads = malloc(sizeof(Load *));
        for (size_t i = 0; i < num_loads; i++) {
            loads[i] = load_from_config(json_array_get(element, i));
        }

        load = new_compound_load(
            loads,
            num_loads,
            load_type
        );

        free(loads);
    }
    else if (load_type == COMPONENT_LOAD) {
        load = new_component_load(component_from_config(element));
    }
    else {
        assert(false);
    }

    return load;
}

Component component_from_config(json_t *component_config) {

    double component_lower_limit_num;
    double component_upper_limit_num;
    double component_value_num;
    char *component_type_str = NULL;
    bool is_connected = true;

    static const char *k_component_config_schema = "{s:F, s:F, s:F, s:s, s?b}";
    json_error_t error;
    if (json_unpack_ex(
        component_config, 
        &error, 0, k_component_config_schema, 
        k_component_lower_limit_key, &component_lower_limit_num,
        k_component_upper_limit_key, &component_upper_limit_num,
        k_component_value_key, &component_value_num,
        k_component_type_key, &component_type_str,
        k_component_is_connected_key, &is_connected
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
    if (! strcmp(component_type, k_component_type_resistor_value)) {
        component_type_code = RESISTOR;
    }
    else if (! strcmp(component_type, k_component_type_capacitor_value)) {
        component_type_code = CAPACITOR;
    }
    else if (! strcmp(component_type, k_component_type_inductor_value)) {
        component_type_code = INDUCTOR;
    }
    else {
        fprintf(stderr, "error: %s is not a valid component type", component_type);
        exit(EXIT_FAILURE);
    }
    return component_type_code;

}

LoadType load_type_string_to_code(const char load_type[]) {
    LoadType load_type_code;
    if (! strcmp(load_type, k_load_type_component_value)) {
        load_type_code = COMPONENT_LOAD;
    }
    else if (! strcmp(load_type, k_load_type_parallel_value)) {
        load_type_code = PARALLEL_LOAD;
    }
    else if (! strcmp(load_type, k_load_type_series_value)) {
        load_type_code = SERIES_LOAD;
    }
    else {
        fprintf(stderr, "error: %s is not a valid load type", load_type);
        exit(EXIT_FAILURE);
    }
    return load_type_code;
}


FilterStageType filter_stage_type_string_to_code(char stage_type[]) {
    FilterStageType stage_type_code;
    if (! strcmp(stage_type, k_filter_stage_type_series_value)) {
        stage_type_code = SERIES_FILTER;
    }
    else if (! strcmp(stage_type, k_filter_stage_type_shunt_value)) {
        stage_type_code = SHUNT_FILTER;
    }
    else {
        fprintf(stderr, "error: %s is not a valid filter stage type", stage_type);
        exit(EXIT_FAILURE);
    }
    return stage_type_code;
}

static void handle_error(json_error_t *error) {
    fprintf(
        stderr, 
        "error: Failed to parse JSON file - %s: "
        "Source - %s: "
        "Line number %d: "
        "Column %d: "
        "Position %d",
        error->text, error->source, error->line, error->column, error->position
    );
    exit(EXIT_FAILURE);
}