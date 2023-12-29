#include <complex.h>
#include <assert.h>
#include <libguile.h>

#include "two_port_network.h"

double complex *matrix_element(int row, int column, TwoPortNetwork *matrix) {
    assert(row > 0);
    assert(column > 0);
    assert(row <= 2);
    assert(column <= 2);

    if (row == 1) {
        if (column == 1) {
            return &matrix->element11;
        }
        else {
            return &matrix->element12;
        }
    }
    else {
        if (column == 1) {
            return &matrix->element21;
        }
        else {
            return &matrix->element22;
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

double complex voltageGain(TwoPortNetwork *matrix) {
    return *matrix_element(1, 1, matrix);
}

TwoPortNetwork *make_two_port_network(void) {
    return scm_gc_malloc(sizeof(TwoPortNetwork), "matrix");
}

void series_connected_network(TwoPortNetwork *matrix, complex impedance) {
    *matrix_element(1, 1, matrix) = 1;
    *matrix_element(1, 2, matrix) = impedance;
    *matrix_element(2, 1, matrix) = 0;
    *matrix_element(2, 2, matrix) = 1;
    return matrix;
}

void shunt_connected_network(TwoPortNetwork *matrix, complex impedance) {
    *matrix_element(1, 1, matrix) = 1;
    *matrix_element(1, 2, matrix) = 0;
    *matrix_element(2, 1, matrix) = 1.0 / impedance;
    *matrix_element(2, 2, matrix) = 1;
    return matrix;
}

void transformer_network(TwoPortNetwork *matrix, double turns_ratio) {
    TwoPortNetwork *matrix = make_two_port_network();
    *matrix_element(1, 1, matrix) = turns_ratio;
    *matrix_element(1, 2, matrix) = 0;
    *matrix_element(2, 1, matrix) = 0;
    *matrix_element(2, 2, matrix) = 1.0 / turns_ratio;
    return matrix;
}

void identity_network(TwoPortNetwork *matrix) {
    return transformer_network(matrix, 1.0);
}
