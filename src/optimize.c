#include <math.h>
#include <assert.h>

#include "filter.h"
#include "random.h"
#include "two_port_network.h"

static const double k_pi = 3.1415926535897932384626433;

typedef struct {
    double initial_temperature;
    double final_temperature;
} CoolingSchedule;

typedef struct {
    unsigned int n_points;
    double start_frequency;
    double end_frequency;
    double target_impedance;
    Filter *source;
} ImpedanceMatchParameters;

Filter *optimize_simulated_annealing(
    const Filter *initial_filter, 
    unsigned long n_evaluations,
    double (*objective)(Filter *filter, void *parameters),
    void *objective_parameters,
    CoolingSchedule cooling_schedule,
    unsigned long prng_seed
) {
    assert(initial_filter != NULL);
    assert(objective != NULL);

    MTRand prng = seedRand(prng_seed);

    Filter *filter_a = duplicate_filter(initial_filter);
    assert(filter_a != NULL);
    Filter *filter_b = duplicate_filter(initial_filter);
    assert(filter_b != NULL);

    Filter **current_filter = &filter_a;
    Filter **candidate_filter = &filter_b;

    double current_energy = objective(*current_filter, objective_parameters);

    for (unsigned long i = 0; i < n_evaluations; i++) {
        filter_random_update(*candidate_filter, &prng);

        int candidate_energy = objective(*candidate_filter, objective_parameters);
        double fraction_complete = 1.0 - (i + 1.0) / (double) n_evaluations;
        double temperature = 
            cooling_schedule.initial_temperature * (1 - fraction_complete) +
            cooling_schedule.final_temperature * fraction_complete;

        double acceptance_probability = 
            candidate_energy < current_energy ? 
            1 : 
            exp(-(candidate_energy - current_energy) / temperature);


        if (
            acceptance_probability >= genRand(&prng)
        ) {
            Filter **next_candidate_filter = current_filter;
            current_filter = candidate_filter;
            candidate_filter = next_candidate_filter;
            current_energy = candidate_energy;
        }
    }

    free_filter_node(*candidate_filter);
    return *current_filter;
}

double impedance_match_objective(
    Filter *filter, ImpedanceMatchParameters *parameters
) {
    assert(filter != NULL);
    assert(parameters != NULL);

    double start_angular_frequency = parameters->start_frequency * 2 * k_pi;
    double end_angular_frequency = parameters->end_frequency * 2 * k_pi;
    double frequency_step = 
        (end_angular_frequency - start_angular_frequency) / parameters->n_points;
    
    double max_impedance_deviation = 0;
    for (
        unsigned int point_count = 0; 
        point_count < parameters->n_points; 
        point_count++
    ) {
        double angular_frequency = 
            start_angular_frequency + frequency_step * point_count;

        double complex matched_impedance = network_output_impedance(
            get_filter_network(angular_frequency, filter),
            network_output_impedance(
                get_filter_network(angular_frequency, parameters->source),
                0.0
            )
        );

        double impedance_deviation = 
            cabs(matched_impedance - parameters->target_impedance);
        if (max_impedance_deviation < impedance_deviation) {
            max_impedance_deviation = impedance_deviation;
        }
    }

    return max_impedance_deviation;
}