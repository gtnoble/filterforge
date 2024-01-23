#include <complex.h>
#include <assert.h>

#include "two_port_network.h"

double complex *matrix_element(int row, int column, TwoPortNetwork *network) {
    assert(row > 0);
    assert(column > 0);
    assert(row <= 2);
    assert(column <= 2);

    return &network->abcd_elements[row - 1][column - 1];
}

TwoPortNetwork cascade_network(TwoPortNetwork matrix1, TwoPortNetwork matrix2) {
    TwoPortNetwork result;
    for (int i = 1; i <= 2; i++) {
        for (int j = 1; j <= 2; j++) {
            for (int k = 1; k <= 2; k++) {
                *matrix_element(i, j, &result) += 
                    *matrix_element(i, k, &matrix1) * 
                    *matrix_element(k, j, &matrix2);
            }
        }
    }
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
    transformer_network(1.0);
}
