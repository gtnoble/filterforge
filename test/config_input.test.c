#include <stdlib.h>
#include <check.h>

#include "config_input.h"
#include "config_output.h"

START_TEST(test_single_component_filter) {
    Filter *filter = load_filter("./test_filter.json");
    ck_assert_ptr_nonnull(filter);

    ck_assert_int_eq(filter->num_stages, 1);
    FilterStage *stage = filter->stages[0];
    ck_assert_ptr_nonnull(stage);
    ck_assert_int_eq(stage->type, SERIES_FILTER);
    Load *load = stage->load;
    ck_assert_ptr_nonnull(load);
    ck_assert_int_eq(load->type, COMPONENT_LOAD);
    Component component = load->element.component;
    ck_assert(preferred_values_equal(component.lower_limit, make_preferred_value(0, 0)));
    ck_assert(preferred_values_equal(component.upper_limit, make_preferred_value(0, 2)));
    ck_assert(preferred_values_equal(component.value, make_preferred_value(0, 1)));
    ck_assert_int_eq(component.type, RESISTOR);
    ck_assert_int_eq(component.is_connected, true);
    
    //@todo clean up memory 

    Component reloaded_component = component_from_config(component_to_config(component));
    ck_assert(components_equal(reloaded_component, component));

    Load *reloaded_load = load_from_config(load_to_config(load));
    ck_assert(loads_equal(reloaded_load, load));

    FilterStage *reloaded_stage = stage_from_config(filter_stage_to_config(stage));
    ck_assert(filter_stages_equal(reloaded_stage, stage));

    Filter *reloaded_filter = filter_from_config(filter_to_config(filter));
    ck_assert(filters_equal(reloaded_filter, filter));

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

int main(void) {
    SRunner *sr = srunner_create(json_input_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}