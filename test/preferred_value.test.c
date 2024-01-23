#include <check.h>
#include <stdlib.h>

#include "preferred_value.h"

START_TEST(test_create_preferred_value) {
    PreferredValue preferred_value =  make_preferred_value(0, 0);
    ck_assert_int_eq(preferred_value.index, 0);
    ck_assert_int_eq(preferred_value.order_of_magnitude, 0);
}
END_TEST

START_TEST(test_copy_preferred_value) {
    PreferredValue source = make_preferred_value(1, 1);
    PreferredValue destination = make_preferred_value(0, 0);
    copy_preferred_value(source, &destination);
    
    ck_assert_int_eq(source.index, destination.index);
    ck_assert_int_eq(source.order_of_magnitude, destination.order_of_magnitude);
}
END_TEST

START_TEST(test_evaluate_preferred_value) {
    PreferredValue value = make_preferred_value(0, 0);
    ck_assert_double_eq_tol(evaluate_preferred_value(value), 1.0, 0.001);
}
END_TEST

START_TEST(test_floor_preferred_value) {
    PreferredValue value = floor_preferred_value(1.01);
    ck_assert_int_eq(value.index, 0);
    ck_assert_int_eq(value.order_of_magnitude, 0);
}
END_TEST

START_TEST(test_ceiling_preferred_value) {
    PreferredValue value = ceiling_preferred_value(1.01);
    ck_assert_int_eq(value.index, 1);
    ck_assert_int_eq(value.order_of_magnitude, 0);
}
END_TEST

START_TEST(test_nearest_preferred_value) {
    PreferredValue value = nearest_preferred_value(1.01);
    ck_assert_int_eq(value.index, 0);
    ck_assert_int_eq(value.order_of_magnitude, 0);

    value = nearest_preferred_value(1.09);
    ck_assert_int_eq(value.index, 1);
    ck_assert_int_eq(value.order_of_magnitude, 0);
}
END_TEST

START_TEST(test_increment_preferred_value) {
    PreferredValue value = make_preferred_value(0, 0);
    increment_preferred_value(&value);
    ck_assert_int_eq(value.index, 1);
    ck_assert_int_eq(value.order_of_magnitude, 0);

    value = make_preferred_value(23, 0);
    increment_preferred_value(&value);
    ck_assert_int_eq(value.index, 0);
    ck_assert_int_eq(value.order_of_magnitude, 1);
}
END_TEST

START_TEST(test_decrement_preferred_value) {
    PreferredValue value = make_preferred_value(0, 0);
    decrement_preferred_value(&value);
    ck_assert_int_eq(value.index, 23);
    ck_assert_int_eq(value.order_of_magnitude, -1);

    value = make_preferred_value(23, 0);
    decrement_preferred_value(&value);
    ck_assert_int_eq(value.index, 22);
    ck_assert_int_eq(value.order_of_magnitude, 0);
}
END_TEST



START_TEST(test_preferred_values_comparison) {
    PreferredValue value1 = make_preferred_value(0, 0);
    PreferredValue value2 = make_preferred_value(0, 0);

    ck_assert(preferred_values_equal(value1, value2));
    ck_assert(preferred_values_greater_than_or_equal(value1, value2));
    ck_assert(preferred_values_less_than_or_equal(value1, value2));

    value1 = make_preferred_value(1, 0);
    ck_assert(! preferred_values_equal(value1, value2));
    ck_assert(preferred_values_greater_than_or_equal(value1, value2));
    ck_assert(preferred_values_less_than_or_equal(value2, value1));

    value1 = make_preferred_value(0, 1);
    ck_assert(! preferred_values_equal(value1, value2));
    ck_assert(preferred_values_greater_than_or_equal(value1, value2));
    ck_assert(preferred_values_less_than_or_equal(value2, value1));
}
END_TEST

Suite *preferred_value_suite(void) {
    Suite *s = suite_create("Preferred Value");

    TCase *main_tests = tcase_create("Core");

    tcase_add_test(main_tests, test_create_preferred_value);
    tcase_add_test(main_tests, test_copy_preferred_value);
    tcase_add_test(main_tests, test_evaluate_preferred_value);
    tcase_add_test(main_tests, test_floor_preferred_value);
    tcase_add_test(main_tests, test_ceiling_preferred_value);
    tcase_add_test(main_tests, test_nearest_preferred_value);
    tcase_add_test(main_tests, test_increment_preferred_value);
    tcase_add_test(main_tests, test_decrement_preferred_value);
    tcase_add_test(main_tests, test_preferred_values_comparison);

    suite_add_tcase(s, main_tests);

    return s;
}

int main(void) {
    SRunner *sr = srunner_create(preferred_value_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
