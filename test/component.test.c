#include <check.h>
#include <stdlib.h>

#include "component.h"
#include "preferred_value.h"
#include "random.h"

Component make_test_component(ComponentType type) {
    PreferredValue component_value = make_preferred_value(0, 0);
    PreferredValue upper_limit = make_preferred_value(0, 1);
    PreferredValue lower_limit = make_preferred_value(0, -1);
    bool is_connected = true;
    return new_component(type, component_value, lower_limit, upper_limit, is_connected);
}

START_TEST(test_new_component) {
    PreferredValue component_value = make_preferred_value(0, 0);
    PreferredValue upper_limit = make_preferred_value(0, 1);
    PreferredValue lower_limit = make_preferred_value(0, -1);
    bool is_connected = true;
    Component component = 
        new_component(RESISTOR, component_value, lower_limit, upper_limit, is_connected);
    
    ck_assert(preferred_values_equal(component_value, component.value));
    ck_assert(preferred_values_equal(upper_limit, component.upper_limit));
    ck_assert(preferred_values_equal(lower_limit, component.lower_limit));
    ck_assert_int_eq(component.is_connected, is_connected);
    ck_assert_int_eq(component.type, RESISTOR);
}
END_TEST

START_TEST(test_copy_component) {
    Component source_component = make_test_component(RESISTOR);
    Component destination_component;

    copy_component(source_component, &destination_component);

    ck_assert(
        preferred_values_equal(source_component.lower_limit, destination_component.lower_limit)
    );
    ck_assert(
        preferred_values_equal(source_component.upper_limit, destination_component.upper_limit)
    );
    ck_assert(
        preferred_values_equal(source_component.value, destination_component.value)
    );
    ck_assert_int_eq(destination_component.type, source_component.type);
    ck_assert_int_eq(destination_component.is_connected, source_component.is_connected);

}
END_TEST

START_TEST(test_component_impedance) {
    Component resistor = make_test_component(RESISTOR);

    double complex dc_resistor_impedance = component_impedance(0, resistor);
    ck_assert_double_eq_tol(creal(dc_resistor_impedance), 1.0, 0.0001);
    ck_assert_double_eq_tol(cimag(dc_resistor_impedance), 0.0, 0.0001);

    double complex ac_resistor_impedance = component_impedance(1, resistor);
    ck_assert_double_eq_tol(creal(ac_resistor_impedance), creal(dc_resistor_impedance), 0.0001);
    ck_assert_double_eq_tol(cimag(ac_resistor_impedance), cimag(dc_resistor_impedance), 0.0001);

    Component capacitor = make_test_component(CAPACITOR);

    double complex ac_capacitor_impedance = component_impedance(1, capacitor);
    ck_assert_double_eq_tol(creal(ac_capacitor_impedance), 0.0, 0.0001);
    ck_assert_double_eq_tol(cimag(ac_capacitor_impedance), -1.0, 0.0001);

    Component inductor = make_test_component(INDUCTOR);

    double complex ac_inductor_impedance = component_impedance(1, inductor);
    ck_assert_double_eq_tol(creal(ac_inductor_impedance), 0.0, 0.0001);
    ck_assert_double_eq_tol(cimag(ac_inductor_impedance), 1.0, 0.0001);

    double complex dc_inductor_impedance = component_impedance(0, inductor);
    ck_assert_double_eq_tol(creal(dc_inductor_impedance), 0.0, 0.0001);
    ck_assert_double_eq_tol(cimag(dc_inductor_impedance), 0.0, 0.0001);
}
END_TEST

START_TEST(test_component_random_update) {
    MTRand prng = seedRand(1);

    Component centered = new_component(
        RESISTOR, 
        make_preferred_value(0, 0),
        make_preferred_value(0, -1),
        make_preferred_value(0, 1),
        true
    );

    Component updated_centered = centered;

    component_random_update(&updated_centered, &prng);
    ck_assert(
        preferred_values_equal(updated_centered.value, make_preferred_value(1, 0)) ||
        preferred_values_equal(updated_centered.value, make_preferred_value(23, -1))
    );

    Component lower_bound = new_component(
        RESISTOR,
        make_preferred_value(0, 0),
        make_preferred_value(0, 0),
        make_preferred_value(0, 1),
        true
    );
    component_random_update(&lower_bound, &prng);

    ck_assert(preferred_values_equal(lower_bound.value, make_preferred_value(1, 0)));

    Component upper_bound = new_component(
        RESISTOR,
        make_preferred_value(0, 1),
        make_preferred_value(0, 0),
        make_preferred_value(0, 1),
        true
    );
    component_random_update(&upper_bound, &prng);

    ck_assert(preferred_values_equal(upper_bound.value, make_preferred_value(23, 0)));
}
END_TEST

Suite *component_suite(void) {
    Suite *s = suite_create("Component");
    TCase *tests = tcase_create("Core");

    tcase_add_test(tests, test_new_component);
    tcase_add_test(tests, test_copy_component);
    tcase_add_test(tests, test_component_impedance);
    tcase_add_test(tests, test_component_random_update);

    suite_add_tcase(s, tests);

    return s;
}

int main(void) {
    SRunner *sr = srunner_create(component_suite());
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}