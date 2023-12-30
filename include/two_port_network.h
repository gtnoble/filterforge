#ifndef FILTOPT_TWO_PORT_NETWORK
#define FILTOPT_TWO_PORT_NETWORK

#include <complex.h>

typedef struct {
    double complex element11;
    double complex element12;
    double complex element21;
    double complex element22;
} TwoPortNetwork;

double complex network_voltage_gain(TwoPortNetwork *matrix);

void cascade_network(TwoPortNetwork *result, TwoPortNetwork *matrix1, TwoPortNetwork *matrix2);
void series_connected_network(TwoPortNetwork *matrix, complex impedance);
void shunt_connected_network(TwoPortNetwork *matrix, complex impedance);
void transformer_network(TwoPortNetwork *matrix, double turns_ratio);
void identity_network(TwoPortNetwork *matrix);

#endif
