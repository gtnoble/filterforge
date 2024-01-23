#include <check.h>
#include <stdlib.h>

#include "load.h"
#include "component.h"

Component make_test_component(void) {
    return new_component(
        RESISTOR, 
        make_preferred_value(0, 0), 
        make_preferred_value(0, -1), 
        make_preferred_value(0, 1),
        true
    );
}

START_TEST(test_new_component_load) {
    Component component = make_test_component();
    Load *component_load = new_component_load(component);

    component_load->type = COMPONENT_LOAD;
    component_load->num_elements = 1;
    Component load_component = component_load->element.component;
    ck_assert(components_equal(load_component, component));

    double complex impedance = load_impedance(0, component_load);
    ck_assert_double_eq_tol(creal(impedance), 1, 0.0001);
    ck_assert_double_eq_tol(cimag(impedance), 0, 0.0001);

}
END_TEST

START_TEST(test_new_compound_load) {
    Component component = make_test_component();

    Load *load1 = new_component_load(component);
    Load *load2 = new_component_load(component);
    Load *loads[] = {load1, load2};

    Load *series_load = new_compound_load(loads, 2, SERIES_LOAD);
    double complex series_impedance = load_impedance(0, series_load);
    ck_assert_double_eq_tol(creal(series_impedance), 2, 0.0001);
    ck_assert_double_eq_tol(cimag(series_impedance), 0, 0.0001);

    Load *parallel_load = new_compound_load(loads, 2, PARALLEL_LOAD);
    double complex parallel_impedance = load_impedance(0, parallel_load);
    ck_assert_double_eq_tol(creal(parallel_impedance), 0.5, 0.0001);
    ck_assert_double_eq_tol(cimag(parallel_impedance), 0, 0.0001);

}

Suite *load_suite(void) {
    Suite *s = suite_create("Load");
    TCase *tests = tcase_create("Core");

    tcase_add_test(tests, test_new_component_load);
    tcase_add_test(tests, test_new_compound_load);

    suite_add_tcase(s, tests);

    return s;
}

int main(void) {
    SRunner *sr = srunner_create(load_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}