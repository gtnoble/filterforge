#include <check.h>
#include <stdlib.h>

#include "load.h"
#include "filter.h"

Component make_test_component() {
    PreferredValue component_value = make_preferred_value(0, 0);
    PreferredValue upper_limit = make_preferred_value(0, 1);
    PreferredValue lower_limit = make_preferred_value(0, -1);
    bool is_connected = true;
    return new_component(RESISTOR, component_value, lower_limit, upper_limit, is_connected);
}

Component make_test_component2() {
    PreferredValue component_value = make_preferred_value(1, 0);
    PreferredValue upper_limit = make_preferred_value(1, 1);
    PreferredValue lower_limit = make_preferred_value(1, -1);
    bool is_connected = false;
    return new_component(CAPACITOR, component_value, lower_limit, upper_limit, is_connected);
}

Load *make_test_load1(void) {
    return new_component_load(make_test_component());
}

Load *make_test_load2(void) {
    return new_component_load(make_test_component2());
}

FilterStage *make_test_stage1(void) {
    return new_filter_stage(make_test_load1(), SHUNT_FILTER);
}

FilterStage *make_test_stage2(void) {
    return new_filter_stage(make_test_load2(), SERIES_FILTER);
}

Filter *make_test_shunt_filter(void) {
    FilterStage *stages[] = {make_test_stage1()};
    return new_filter(stages, 1);
}

Filter *make_test_cascade_filter(void) {
    FilterStage *stages[] = {make_test_stage1(), make_test_stage2()};
    return new_filter(stages, 2);
}

Filter *make_test_series_filter(void) {
    FilterStage *stages[] = {make_test_stage2()};
    return new_filter(stages, 1);
}

START_TEST(test_make_filter_stage) {
    FilterStage *stage = make_test_stage1();
    Load *load = make_test_load1();
    ck_assert_ptr_nonnull(stage);

    ck_assert(loads_equal(stage->load, load));
    ck_assert_int_eq(stage->type, SHUNT_FILTER);

    free_filter_stage_node(stage);
    free_load_node(load);
}
END_TEST

START_TEST(test_filter_stages_equal) {
    FilterStage *stage1 = make_test_stage1();
    FilterStage *stage2 = make_test_stage1();

    ck_assert(filter_stages_equal(stage1, stage2));

    free_filter_stage_node(stage1);
    free_filter_stage_node(stage2);
}
END_TEST

START_TEST(test_duplicate_filter_stage) {
    FilterStage *stage = make_test_stage1();
    ck_assert_ptr_nonnull(stage);
    FilterStage *duplicate_stage = duplicate_filter_stage(stage);
    ck_assert_ptr_nonnull(duplicate_stage);

    ck_assert(filter_stages_equal(stage, duplicate_stage));

    free_filter_stage_node(stage);
    free_filter_stage_node(duplicate_stage);
}
END_TEST

START_TEST(test_copy_filter_stage) {
    FilterStage *stage1 = make_test_stage1();
    FilterStage *stage2 = make_test_stage2();
    ck_assert(! filter_stages_equal(stage1, stage2));

    copy_filter_stage(stage1, stage2);
    ck_assert(filter_stages_equal(stage1, stage2));

    free_filter_stage_node(stage1);
    free_filter_stage_node(stage2);
}
END_TEST

START_TEST(test_filter_stage_random_update) {
    FilterStage *stage = make_test_stage1();
    FilterStage *stage_duplicate = make_test_stage1();
    ck_assert(filter_stages_equal(stage, stage_duplicate));
    MTRand prng = seedRand(1);

    filter_stage_random_update(stage_duplicate, &prng);
    ck_assert(! filter_stages_equal(stage, stage_duplicate));

    copy_filter_stage(stage, stage_duplicate);
    ck_assert(filter_stages_equal(stage, stage_duplicate));

    free_filter_stage_node(stage);
    free_filter_stage_node(stage_duplicate);
}

START_TEST(test_new_filter) {
    Filter *filter = make_test_shunt_filter();
    FilterStage *stage = make_test_stage1();
    ck_assert_int_eq(filter->num_stages, 1);

    ck_assert(filter_stages_equal(filter->stages[0], stage));

    free_filter_node(filter);
    free_filter_stage_node(stage);
}
END_TEST

START_TEST(test_filters_equal) {
    Filter *filter1 = make_test_shunt_filter();
    Filter *filter2 = make_test_shunt_filter();

    ck_assert(filters_equal(filter1, filter2));

    free_filter_node(filter1);
    free_filter_node(filter2);
}

START_TEST(test_duplicate_filter) {
    Filter *filter = make_test_shunt_filter();
    Filter *duplicated_filter = duplicate_filter(filter);

    ck_assert(filters_equal(filter, duplicated_filter));

    free_filter_node(filter);
    free_filter_node(duplicated_filter);
}
END_TEST

START_TEST(test_copy_filter) {
    Filter *filter = make_test_shunt_filter();
    Filter *filter_copy = make_test_series_filter();
    ck_assert(! filters_equal(filter, filter_copy));

    copy_filter(filter, filter_copy);
    ck_assert(filters_equal(filter, filter_copy));

    free_filter_node(filter);
    free_filter_node(filter_copy);
}
END_TEST

START_TEST(test_filter_random_update) {
    Filter *filter = make_test_shunt_filter();
    Filter *filter_copy = make_test_shunt_filter();
    ck_assert(filters_equal(filter, filter_copy));

    MTRand prng = seedRand(1);
    filter_random_update(filter_copy, &prng);

    ck_assert(! filters_equal(filter, filter_copy));

    free_filter_node(filter);
    free_filter_node(filter_copy);
}
END_TEST

START_TEST(test_filter_voltage_gain) {
    Filter *filter = make_test_shunt_filter();

    ck_assert_double_eq_tol(filter_voltage_gain(0, filter), 1, 0.0001);

    free_filter_node(filter);
}
END_TEST

START_TEST(test_filter_impedance) {
    Filter *filter = make_test_shunt_filter();

    ck_assert_double_eq_tol(creal(filter_output_impedance(0, filter, 0)), 0, 0.0001);
    ck_assert_double_eq_tol(cimag(filter_output_impedance(0, filter, 0)), 0, 0.0001);
    free_filter_node(filter);

    Filter *cascade_filter = make_test_cascade_filter();
    ck_assert_double_eq_tol(
        creal(filter_output_impedance(0, cascade_filter, 0)), 10, 0.00001
    );
    ck_assert_double_eq_tol(
        cimag(filter_output_impedance(0, cascade_filter, 0)), 0, 0.00001
    );

    ck_assert_double_eq_tol(
        creal(filter_input_impedance(0, cascade_filter, INFINITY)), 1, 0.00001
    );
    ck_assert_double_eq_tol(
        cimag(filter_input_impedance(0, cascade_filter, INFINITY)), 0, 0.00001
    );

}
END_TEST

Suite *filter_stage_suite() {
    Suite *s = suite_create("FilterStage");
    TCase *tests = tcase_create("Core");

    tcase_add_test(tests, test_make_filter_stage);
    tcase_add_test(tests, test_filter_stages_equal);
    tcase_add_test(tests, test_duplicate_filter_stage);
    tcase_add_test(tests, test_copy_filter_stage);
    tcase_add_test(tests, test_filter_stage_random_update);

    suite_add_tcase(s, tests);

    return s;
}

Suite *filter_suite() {
    Suite *s = suite_create("Filter");
    TCase *tests = tcase_create("Core");

    tcase_add_test(tests, test_new_filter);
    tcase_add_test(tests, test_filters_equal);
    tcase_add_test(tests, test_duplicate_filter);
    tcase_add_test(tests, test_copy_filter);
    tcase_add_test(tests, test_filter_random_update);
    tcase_add_test(tests, test_filter_voltage_gain);
    tcase_add_test(tests, test_filter_impedance);

    suite_add_tcase(s, tests);

    return s;
}

int main(void) {
    SRunner *sr = srunner_create(filter_stage_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    sr = srunner_create(filter_suite());
    srunner_run_all(sr, CK_NORMAL);
    number_failed += srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}