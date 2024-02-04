#include <complex.h>
#include <assert.h>
#include <stdlib.h>

#include "two_port_network.h"

void matrix_matrix_multiply(
    const double complex (*matrix1)[2][2], 
    const double complex (*matrix2)[2][2],
    double complex (*result_matrix)[2][2]
) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                (*result_matrix)[i][j] += 
                    (*matrix1)[i][k] * 
                    (*matrix2)[k][j];
            }
        }
    }
}

void scalar_matrix_multiply(
    double complex scalar,
    const double complex (*matrix)[2][2],
    double complex (*result)[2][2]
) {
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            (*result)[i][j] = scalar * (*matrix)[i][j];
        }
    }
}

double complex determinant(const double complex (*matrix)[2][2]) {
    return (*matrix)[0][0] * (*matrix)[1][1] - (*matrix)[0][1] * (*matrix)[1][0];
}

void network_impedance_parameters(TwoPortNetwork network, double complex (*result)[2][2]) {
    (*result)[0][0] = network.abcd_elements[0][0];
    (*result)[1][1] = network.abcd_elements[1][1];
    (*result)[0][1] = determinant(&network.abcd_elements);
    (*result)[1][0] = 1;
    scalar_matrix_multiply(1.0 / network.abcd_elements[1][0], result, result);
}

double complex network_output_impedance(
    TwoPortNetwork network, 
    double complex source_impedance
) {
    double complex impedance_parameters[2][2];
    network_impedance_parameters(network, &impedance_parameters);

    return 
        impedance_parameters[1][1] - 
        (impedance_parameters[0][1] * impedance_parameters[1][0]) /
        (impedance_parameters[0][0] + source_impedance);
}

double complex network_input_impedance(
    TwoPortNetwork network,
    double complex load_impedance
) {

    double complex impedance_parameters[2][2];
    network_impedance_parameters(network, &impedance_parameters);

    return 
        impedance_parameters[0][0] -
        (impedance_parameters[0][1] * impedance_parameters[1][0]) /
        (impedance_parameters[1][1] + load_impedance);
}

TwoPortNetwork cascade_network(TwoPortNetwork matrix1, TwoPortNetwork matrix2) {
    TwoPortNetwork result;
    matrix_matrix_multiply(
        &matrix1.abcd_elements, &matrix2.abcd_elements, &result.abcd_elements
    );
    return result;
}

double complex network_voltage_gain(TwoPortNetwork network) {
    return network.abcd_elements[0][0];
}

TwoPortNetwork series_connected_network(double complex impedance) {
    TwoPortNetwork result;
    result.abcd_elements[0][0] = 1;
    result.abcd_elements[0][1] = impedance;
    result.abcd_elements[1][0] = 0;
    result.abcd_elements[1][1] = 1;
    return result;
}

TwoPortNetwork shunt_connected_network(double complex impedance) {
    TwoPortNetwork result;
    result.abcd_elements[0][0] = 1;
    result.abcd_elements[0][1] = 0;
    result.abcd_elements[1][0] = 1.0 / impedance;
    result.abcd_elements[1][1] = 1;
    return result;
}

TwoPortNetwork transformer_network(double turns_ratio) {
    TwoPortNetwork result;
    result.abcd_elements[0][0] = turns_ratio;
    result.abcd_elements[0][1] = 0;
    result.abcd_elements[1][0] = 0;
    result.abcd_elements[1][1] = 1.0 / turns_ratio;
    return result;
}

TwoPortNetwork identity_network() {
    return transformer_network(1.0);
}
