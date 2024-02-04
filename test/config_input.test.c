#include <stdlib.h>
#include <check.h>
#include <assert.h>

#include "config_input.h"
#include "config_output.h"

char *filename;

START_TEST(test_single_component_filter) {
    Filter *filter = load_filter(filename);
    ck_assert_ptr_nonnull(filter);

    ck_assert_int_eq(filter->num_stages, 1);
    FilterStage *stage = filter->stages[0];
    ck_assert_ptr_nonnull(stage);
    ck_assert_int_eq(stage->type, SERIES_FILTER);
    Load *load = stage->load;
    ck_assert_ptr_nonnull(load);
    ck_assert_int_eq(load->type, PARALLEL_LOAD);
    Load *parallel_load_element = load->element.loads[0];
    Component component = parallel_load_element->element.component;
    ck_assert(preferred_values_equal(component.lower_limit, make_preferred_value(0, 0)));
    ck_assert(preferred_values_equal(component.upper_limit, make_preferred_value(0, 2)));
    ck_assert(preferred_values_equal(component.value, make_preferred_value(0, 1)));
    ck_assert_int_eq(component.type, RESISTOR);
    ck_assert_int_eq(component.is_connected, true);
    
    json_t *generated_component = component_to_config(component);
    Component reloaded_component = component_from_config(generated_component);
    ck_assert(components_equal(reloaded_component, component));
    json_decref(generated_component);

    json_t *generated_load = load_to_config(load);
    Load *reloaded_load = load_from_config(generated_load);
    ck_assert(loads_equal(reloaded_load, load));
    json_decref(generated_load);

    json_t *generated_stage = filter_stage_to_config(stage);
    FilterStage *reloaded_stage = stage_from_config(generated_stage);
    ck_assert(filter_stages_equal(reloaded_stage, stage));
    json_decref(generated_stage);

    json_t *generated_filter = filter_to_config(filter);
    Filter *reloaded_filter = filter_from_config(generated_filter);
    ck_assert(filters_equal(reloaded_filter, filter));
    json_decref(generated_filter);

    save_filter("./test_filter_output.json", filter);
    Filter *imported_filter = load_filter("./test_filter_output.json");
    ck_assert(filters_equal(filter, imported_filter));

    free_filter_node(filter);
    free_filter_node(imported_filter);
}
END_TEST

Suite *json_input_suite() {
    Suite *s = suite_create("JSON Input");
    TCase *tests = tcase_create("Core");

    tcase_add_test(tests, test_single_component_filter);

    suite_add_tcase(s, tests);

    return s;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    filename = argv[1];

    SRunner *sr = srunner_create(json_input_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}