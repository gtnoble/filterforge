#include <jansson.h>
#include <string.h>
#include <assert.h>

#include "component.h"
#include "filter.h"
#include "config.h"
#include "config_output.h"

void component_type_code_to_string(ComponentType type_code, char type_str[]);
void filter_stage_type_code_to_string(FilterStageType type_code, char type_str[]);
void load_type_code_to_string(LoadType type_code, char type_str[]);
static void handle_error(json_error_t *error);

void save_filter(const char filename[], Filter *filter) {
    json_t *filter_config = filter_to_config(filter);
    if (json_dump_file(filter_config, filename, JSON_INDENT(2)) == -1) {
        fprintf(stderr, "Failed to save filter to JSON file");
        exit(EXIT_FAILURE);
    }
    json_decref(filter_config);
}

json_t *component_to_config(Component component) {
    char component_type[20];
    component_type_code_to_string(component.type, component_type);

    json_error_t error;
    json_t *component_config = json_pack_ex(
        &error,
        0,
        "{s:f, s:f, s:f, s:s, s:b}",
        k_component_lower_limit_key, evaluate_preferred_value(component.lower_limit),
        k_component_upper_limit_key, evaluate_preferred_value(component.upper_limit),
        k_component_value_key, evaluate_preferred_value(component.value),
        k_component_type_key, component_type,
        k_component_is_connected_key, component.is_connected
    );
    if (component_config == NULL) {
        handle_error(&error);
    }
    return component_config;
}

json_t *load_to_config(Load *load) {
    assert(load != NULL);
    char load_type[20];
    load_type_code_to_string(load->type, load_type);

    json_t *load_element;
    if (load->type == COMPONENT_LOAD) {
        load_element = component_to_config(load->element.component);
        assert(load_element != NULL);
    }
    else if (load->type == SERIES_LOAD || load->type == PARALLEL_LOAD) {
        assert(load->num_elements > 0);
        load_element = json_array();
        if (load_element == NULL) {
            fprintf(
                stderr,
                "Error: failed to allocate configuration load elements"
            );
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < load->num_elements; i++) {
            if (json_array_append_new(load_element, load_to_config(load->element.loads[i])) == -1) {
                fprintf(
                    stderr,
                    "Error: failed to append load element to configuration"
                );
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        assert(false);
    }

    json_error_t error;
    json_t *load_configuration = json_pack_ex(
        &error,
        0,
        "{s:s, s:o}",
        k_load_type_key, load_type,
        k_load_element_key, load_element
    );

    if (load_configuration == NULL) {
        handle_error(&error);
    }

    return load_configuration;
}

json_t *filter_stage_to_config(FilterStage *stage) {
    char filter_stage_type[20];
    filter_stage_type_code_to_string(stage->type, filter_stage_type);

    json_t *load_configuration = load_to_config(stage->load);
    assert(load_configuration != NULL);

    json_error_t error;
    json_t *stage_configuration = json_pack_ex(
        &error,
        0,
        "{s:s, s:o}",
        k_filter_stage_type_key, filter_stage_type,
        k_filter_stage_load_key, load_configuration
    );

    if (stage_configuration == NULL) {
        handle_error(&error);
    }

    return stage_configuration;
}

json_t *filter_to_config(Filter *filter) {
    json_t *filter_config = json_array();

    if (filter_config == NULL) {
        fprintf(stderr, "error: failed to allocate filter configuration");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < filter->num_stages; i++) {
        json_t *stage_config = filter_stage_to_config(filter->stages[i]);
        assert(stage_config != NULL);

        if (json_array_append_new(filter_config, stage_config) == -1) {
            fprintf(
                stderr, 
                "error: failed to append filter stage to filter configuration."
            );
        }
    }

    return filter_config;
}


void component_type_code_to_string(ComponentType type_code, char type_str[]) {
    switch (type_code) {
        case RESISTOR:
            strcpy(type_str, k_component_type_resistor_value);
            break;
        case CAPACITOR:
            strcpy(type_str, k_component_type_capacitor_value);
            break;
        case INDUCTOR:
            strcpy(type_str, k_component_type_inductor_value);
            break;
        default:
            assert(false);
    }
}

void filter_stage_type_code_to_string(FilterStageType type_code, char type_str[]) {
    switch (type_code) {
        case SERIES_FILTER:
            strcpy(type_str, k_filter_stage_type_series_value);
            break;
        case SHUNT_FILTER:
            strcpy(type_str, k_filter_stage_type_shunt_value);
            break;
        default:
            assert(false);
    }
}

void load_type_code_to_string(LoadType type_code, char type_str[]) {
    switch (type_code) {
        case COMPONENT_LOAD:
            strcpy(type_str, k_load_type_component_value);
            break;
        case SERIES_LOAD:
            strcpy(type_str, k_load_type_series_value);
            break;
        case PARALLEL_LOAD:
            strcpy(type_str, k_load_type_parallel_value);
            break;
        default:
            assert(false);
    }
}

static void handle_error(json_error_t *error) {
    fprintf(
        stderr, 
        "error: Failed to build JSON object - %s: "
        "Source - %s: "
        "Line number %d: "
        "Column %d: "
        "Position %d",
        error->text, error->source, error->line, error->column, error->position
    );
    exit(EXIT_FAILURE);
}