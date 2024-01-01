#include <complex.h>
#include <assert.h>
#include <libguile.h>

#include "two_port_network.h"

double complex *matrix_element(int row, int column, TwoPortNetwork *network) {
    assert(row > 0);
    assert(column > 0);
    assert(row <= 2);
    assert(column <= 2);

    if (row == 1) {
        if (column == 1) {
            return &network->element11;
        }
        else {
            return &network->element12;
        }
    }
    else {
        if (column == 1) {
            return &network->element21;
        }
        else {
            return &network->element22;
        }
    }
}

void cascade_network(TwoPortNetwork *result, TwoPortNetwork *matrix1, TwoPortNetwork *matrix2) {
    for (int i = 1; i <= 2; i++) {
        for (int j = 1; j <= 2; j++) {
            for (int k = 1; k <= 2; k++) {
                *matrix_element(i, j, result) += 
                    *matrix_element(i, k, matrix1) * 
                    *matrix_element(k, j, matrix2);
            }
        }
    }
}

double complex network_voltage_gain(TwoPortNetwork *network) {
    return *matrix_element(1, 1, network);
}

TwoPortNetwork *make_two_port_network(void) {
    return scm_gc_malloc(sizeof(TwoPortNetwork), "matrix");
}

void series_connected_network(TwoPortNetwork *network, complex impedance) {
    *matrix_element(1, 1, network) = 1;
    *matrix_element(1, 2, network) = impedance;
    *matrix_element(2, 1, network) = 0;
    *matrix_element(2, 2, network) = 1;
}

void shunt_connected_network(TwoPortNetwork *network, complex impedance) {
    *matrix_element(1, 1, network) = 1;
    *matrix_element(1, 2, network) = 0;
    *matrix_element(2, 1, network) = 1.0 / impedance;
    *matrix_element(2, 2, network) = 1;
}

void transformer_network(TwoPortNetwork *network, double turns_ratio) {
    *matrix_element(1, 1, network) = turns_ratio;
    *matrix_element(1, 2, network) = 0;
    *matrix_element(2, 1, network) = 0;
    *matrix_element(2, 2, network) = 1.0 / turns_ratio;
}

void identity_network(TwoPortNetwork *network) {
    transformer_network(network, 1.0);
}
