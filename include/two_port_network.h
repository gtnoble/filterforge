#ifndef FILTOPT_TWO_PORT_NETWORK
#define FILTOPT_TWO_PORT_NETWORK

#include <complex.h>

typedef struct {
    double complex abcd_elements[2][2];
} TwoPortNetwork;

void matrix_matrix_multiply(
    const double complex (*matrix1)[2][2], 
    const double complex (*matrix2)[2][2],
    double complex (*result_matrix)[2][2]
);

void scalar_matrix_multiply(
    double complex scalar,
    const double complex (*matrix)[2][2],
    double complex (*result)[2][2]
);

double complex determinant(const double complex (*matrix)[2][2]);

bool matrices_equal(
    const double complex (*matrix1)[2][2], 
    const double complex (*matrix2)[2][2], 
    double tolerance
);

void network_impedance_parameters(TwoPortNetwork network, double complex (*result)[2][2]);

double complex network_voltage_gain(TwoPortNetwork matrix);
double complex network_output_impedance(
    TwoPortNetwork network, 
    double complex source_impedance
);

double complex network_input_impedance(
    TwoPortNetwork network,
    double complex load_impedance
);

TwoPortNetwork cascade_network(TwoPortNetwork matrix1, TwoPortNetwork matrix2);
TwoPortNetwork series_connected_network(double complex impedance);
TwoPortNetwork shunt_connected_network(double complex impedance);
TwoPortNetwork transformer_network(double turns_ratio);
TwoPortNetwork identity_network();

#endif
