#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "preferred_value.h"

const double e24_values[] = {
    1.0, 1.1, 1.2, 
    1.3, 1.5, 1.6, 
    1.8, 2.0, 2.2, 
    2.4, 2.7, 3.0, 
    3.3, 3.6, 3.9, 
    4.3, 4.7, 5.1, 
    5.6, 6.2, 6.8, 
    7.5, 8.2, 9.1
};

const int num_e24_values = 24;

void increment_preferred_value(PreferredValue *value);
void decrement_preferred_value(PreferredValue *value);

bool preferred_values_equal(PreferredValue *value1, PreferredValue *value2);
bool preferrred_values_less_than(PreferredValue *value1, PreferredValue *value2);
bool preferred_values_less_than_or_equal(
    PreferredValue *value1, PreferredValue *value2
);
bool preferred_values_less_than_or_equal(
    PreferredValue *value1, PreferredValue *value2
);

PreferredValue *make_preferred_value(int value_index, int order_of_magnitude, void *allocate(size_t)) {
    PreferredValue *preferred_value = allocate(sizeof(PreferredValue));
    if (preferred_value == NULL)
        return NULL;
    preferred_value->index = value_index;
    preferred_value->order_of_magnitude = order_of_magnitude;
    return preferred_value;
}

void free_preferred_value(PreferredValue *value, void deallocate(void *)) {
    deallocate(value);
}

PreferredValue *duplicate_preferred_value(PreferredValue *preferred_value, void *allocate(size_t)) {
    return make_preferred_value(
        get_preferred_value_index(preferred_value),
        get_preferred_value_order_of_magnitude(preferred_value),
        allocate
    );
}

void copy_preferred_value(PreferredValue *source, PreferredValue *destination) {
    destination->index = source->index;
    destination->order_of_magnitude = source->order_of_magnitude;
}

double evaluate_preferred_value(PreferredValue *preferred_value) {
    return pow(10.0, preferred_value->order_of_magnitude) * e24_values[preferred_value->index];
}

void floor_preferred_value(double numeric_value, PreferredValue *output) {
    assert(numeric_value > 0);

    double log_value = log10(numeric_value);
    int order_of_magnitude = (int) floor(log_value);
    size_t eseries_index;
    for (size_t i = 0; i < num_e24_values; i++) {
        if (log_value - order_of_magnitude >= log10(e24_values[i])) {
            eseries_index = i;
        }
    }

    output->index = eseries_index;
    output->order_of_magnitude = order_of_magnitude;

    assert(evaluate_preferred_value(output) <= numeric_value);
}

void ceiling_preferred_value(double numeric_value, PreferredValue *output) {
    assert(numeric_value > 0);

    double log_value = log10(numeric_value);
    int order_of_magnitude = (int) floor(log_value);
    size_t eseries_index;
    for (size_t i = num_e24_values - 1; i >= 0; i--) {
        if (log_value - order_of_magnitude <= log10(e24_values[i])) {
            eseries_index = i;
        }
    }

    output->index = eseries_index;
    output->order_of_magnitude = order_of_magnitude;

    assert(evaluate_preferred_value(output) >= numeric_value);
}

void nearest_preferred_value(double value_num, PreferredValue *output) {
    assert(value_num > 0);

    PreferredValue floor;
    floor_preferred_value(value_num, &floor);

    PreferredValue ceiling;
    ceiling_preferred_value(value_num, &ceiling);

    if (
        fabs(evaluate_preferred_value(&floor) - value_num) <= 
        fabs(evaluate_preferred_value(&ceiling) - value_num)
    ) {
        *output = floor;
    }
    else {
        *output = ceiling;
    }
}

void increment_preferred_value(PreferredValue *value) {
    PreferredValue current_value = *value;
    PreferredValue next_value;

    if (current_value.index == num_e24_values - 1) {
        next_value.index = 0;
        next_value.order_of_magnitude = current_value.order_of_magnitude + 1;
    }
    else {
        next_value.index = current_value.index + 1;
        next_value.order_of_magnitude = current_value.order_of_magnitude;
    }

    assert(evaluate_preferred_value(&current_value) < evaluate_preferred_value(&next_value));

    *value = next_value;
}

void decrement_preferred_value(PreferredValue *value) {
    PreferredValue current_value = *value;
    PreferredValue next_value;

    if (current_value.index == 0) {
        next_value.index = num_e24_values - 1;
        next_value.order_of_magnitude = current_value.order_of_magnitude - 1;
    }
    else {
        next_value.index = current_value.index - 1;
        next_value.order_of_magnitude = current_value.order_of_magnitude;
    }

    assert(evaluate_preferred_value(&current_value) > evaluate_preferred_value(&next_value));

    *value = next_value;
}

bool preferred_values_equal(PreferredValue *value1, PreferredValue *value2) {
    return 
        (value1->index == value2->index && 
        value1->order_of_magnitude == value2->order_of_magnitude);
}

bool preferrred_values_less_than(PreferredValue *value1, PreferredValue *value2) {
    if (value1->order_of_magnitude < value2->order_of_magnitude) {
        return true;
    }
    else if (
        value1->order_of_magnitude == value2->order_of_magnitude && 
        value1->index < value2->index
    ) {
        return true;
    }
    return false;
}

bool preferred_values_greater_than(PreferredValue *value1, PreferredValue *value2) {
    return !preferred_values_equal(value1, value2) && !preferrred_values_less_than(value1,value2);
}

bool preferred_values_greater_than_or_equal(
    PreferredValue *value1, PreferredValue *value2
) {
    return 
        preferred_values_equal(value1, value2) || 
        preferred_values_greater_than(value1, value2);
}

bool preferred_values_less_than_or_equal(
    PreferredValue *value1, PreferredValue *value2
) {
    return 
        preferred_values_equal(value1, value2) || 
        preferrred_values_less_than(value1, value2);
}

