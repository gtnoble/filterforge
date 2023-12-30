#include <stdlib.h>
#include <assert.h>
#include <math.h>

double average_value(double *samples, size_t num_samples) {
    assert(samples != NULL);
    assert(num_samples > 0);

    double sum = 0;
    for (size_t i = 0; i < num_samples; i++) {
        sum += samples[i];
    }
    return sum / num_samples;
}

double max_value(double *samples, size_t num_samples) {
    assert(samples != NULL);
    assert(num_samples > 0);

    double max = samples[0];
    for (size_t i = 1; i < num_samples; i++) {
        if (samples[i] > max) {
            max = samples[i];
        }
    }
    return max;
}

double min_value(double *samples, size_t num_samples) {
    assert(samples != NULL);
    assert(num_samples > 0);

    double min = samples[0];
    for (size_t i = 1; i < num_samples; i++) {
        if (samples[i] < min) {
            min = samples[i];
        }
    }
    return min;
}

double max_deviation(double *samples, size_t num_samples) {
    assert(samples != NULL);
    assert(num_samples > 0);

    return max_value(samples, num_samples) - min_value(samples, num_samples);
}
