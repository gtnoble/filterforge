#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "measurement.h"

const double k_tolerance = 0.001;

Measurement test_measurements[] = {
    {.frequency = 2, .value = 2},
    {.frequency = 0, .value = 0},
    {.frequency = 1, .value = 1}
};

static bool approx_equal(double complex a, double complex b) {
    return cabs(a - b) <= k_tolerance;
}

static bool is_cplx_nan(double complex x) {
    return isnan(creal(x)) && isnan(cimag(x));
}

void test_measurements_equal() {
    assert(measurements_equal(test_measurements[0], test_measurements[0]));
    assert(measurements_equal(test_measurements[1], test_measurements[1]));
    assert(measurements_equal(test_measurements[2], test_measurements[2]));
}

void test_copy_measurements() {
    Measurement *copied_measurements = malloc(sizeof(Measurement) * 3);
    copy_measurements(test_measurements, copied_measurements, 3);

    assert(measurements_equal(copied_measurements[0], test_measurements[0]));
    assert(measurements_equal(copied_measurements[1], test_measurements[1]));
    assert(measurements_equal(copied_measurements[2], test_measurements[2]));
}

void test_sort_measurements() {
    Measurement *sorted_measurements = malloc(sizeof(Measurement) * 3);
    copy_measurements(test_measurements, sorted_measurements, 3);
    sort_measurements(sorted_measurements, 3);

    assert(sorted_measurements[0].frequency == 0);
    assert(sorted_measurements[1].frequency == 1);
    assert(sorted_measurements[2].frequency == 2);
}

void test_interpolate_measurements() {
    Measurement *sorted_measurements = malloc(sizeof(Measurement) * 3);
    copy_measurements(test_measurements, sorted_measurements, 3);
    sort_measurements(sorted_measurements, 3);

    assert(approx_equal(interpolate(sorted_measurements, 3, 0), 0));
    assert(approx_equal(interpolate(sorted_measurements, 3, 1), 1));
    assert(approx_equal(interpolate(sorted_measurements, 3, 2), 2));

    assert(approx_equal(interpolate(sorted_measurements, 3, 0.5), 0.5));
    assert(approx_equal(interpolate(sorted_measurements, 3, 1.5), 1.5));

    assert(is_cplx_nan(interpolate(sorted_measurements, 3, 2.5)));
}

int main(void) {
    test_measurements_equal();
    test_copy_measurements();
    test_sort_measurements();
    test_interpolate_measurements();

    return EXIT_SUCCESS;
}

