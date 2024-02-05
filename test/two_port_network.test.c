#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "two_port_network.h"

const double complex k_ones[2][2] = {{1, 1}, {1, 1}};
const double complex k_identity[2][2] = {{1, 0}, {0, 1}};
const double complex k_1234_matrix[2][2] = {{1, 2}, {3, 4}};
const double tolerance = 0.0001;

void test_matrices_equal() {
    assert(matrices_equal(&k_ones, &k_ones, tolerance));
    assert(matrices_equal(&k_1234_matrix, &k_1234_matrix, tolerance));
    assert(! matrices_equal(&k_1234_matrix, &k_identity, tolerance));
}

void test_matrix_matrix_multiply() {
    double complex result[2][2];

    matrix_matrix_multiply(&k_1234_matrix, &k_identity, &result);
    assert(matrices_equal(&result, &k_1234_matrix, tolerance));

    matrix_matrix_multiply(&k_1234_matrix, &k_1234_matrix, &result);
    double complex expected_product[2][2] = {{7, 10}, {15, 22}};
    assert(matrices_equal(&result, &expected_product, tolerance));
}

void test_scalar_matrix_multiply() {
    double complex result[2][2];

    scalar_matrix_multiply(2.0, &k_1234_matrix, &result);
    double complex expected[2][2] = {{2, 4}, {6, 8}};
    assert(matrices_equal(&result, &expected, tolerance));
}

void test_determinant() {
    double complex result = determinant(&k_1234_matrix);
    double complex expected = -2;
    assert(cabs(result - expected) <= 0.001);
}

void test_network_impedance() {
    TwoPortNetwork network;
    memcpy(&network.abcd_elements, &k_1234_matrix, sizeof(k_1234_matrix));

    double complex impedance_parameters[2][2];
    double complex expected_parameters[2][2] = {{1.0 / 3.0, - 2.0 / 3.0}, {1.0 / 3.0, 4.0 / 3.0}};
    network_impedance_parameters(network, &impedance_parameters);
    assert(matrices_equal(&impedance_parameters, &expected_parameters, 0.001));

    double complex expected_output_impedance = 2.0;
    double complex output_impedance = network_output_impedance(network, 0);
    assert(cabs(output_impedance - expected_output_impedance) <= 0.001);

}

int main(void) {
    test_matrices_equal();
    test_matrix_matrix_multiply();
    test_scalar_matrix_multiply();
    test_determinant();
    test_network_impedance();
    return EXIT_SUCCESS;
}