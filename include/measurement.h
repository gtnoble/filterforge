
#include <stdlib.h>
#include <complex.h>

typedef struct {
    double complex value;
    double frequency;
} Measurement;

double complex interpolate(Measurement points[], int size, double frequency);
void sort_measurements(Measurement measurements[], size_t n_measurements);
bool measurements_equal(Measurement measurement1, Measurement measurement2);
void copy_measurements(const Measurement *source, Measurement *destination, size_t n_measurements);