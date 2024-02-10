
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "measurement.h"

double complex linear_interpolation(double frequency, Measurement p1, Measurement p2);
int measurement_compare(const void *measurement_1, const void *measurement_2);

void sort_measurements(Measurement measurements[], size_t n_measurements) {
    qsort(measurements, n_measurements, sizeof(Measurement), measurement_compare);
}

int measurement_compare(const void *measurement_1, const void *measurement_2) {
    double difference = (
        ((Measurement *) measurement_2)->frequency - 
        ((Measurement *) measurement_1)->frequency);

    if (difference > 0)
        return -1;
    else if (difference < 0)
        return 1;
    else return 0;
}

void copy_measurements(const Measurement *source, Measurement *destination, size_t n_measurements) {
    for (size_t i = 0; i < n_measurements; i++) {
        destination[i] = source[i];
    }
}

bool measurements_equal(Measurement measurement1, Measurement measurement2) {
    return 
        measurement1.frequency == measurement2.frequency &&
        measurement1.value == measurement2.value;
}

// Linear interpolation function
double complex linear_interpolation(double frequency, Measurement p1, Measurement p2) {
    return p1.value + (frequency - p1.frequency) * (p2.value - p1.value) / (p2.frequency - p1.frequency);
}

// Function to perform linear interpolation on a sorted array of points
double complex interpolate(Measurement points[], int size, double frequency) {
    // If frequency is beyond the range of points, return an error or handle accordingly
    if (frequency < points[0].frequency || frequency > points[size - 1].frequency) {
        fprintf(stderr, "Error: Out of range\n");
        return CMPLX(NAN, NAN);
    }

    // Binary search to find the appropriate interval [i, i+1] for the given frequency
    int low = 0, high = size - 1;
    while (low < high) {
        int mid = (low + high) / 2;
        if (points[mid].frequency < frequency) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }


    // Perform linear interpolation between points[low] and points[low-1]
    return linear_interpolation(frequency, points[low - 1], points[low]);
}