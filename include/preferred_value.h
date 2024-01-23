#ifndef FILTERFORGE_PREFERRED_VALUE
#define FILTERFORGE_PREFERRED_VALUE

#include <stdbool.h>

typedef struct {
    size_t index;
    int order_of_magnitude;
} PreferredValue;

PreferredValue make_preferred_value(int value_index, int order_of_magnitude);
void copy_preferred_value(PreferredValue source, PreferredValue *destination);
double evaluate_preferred_value(PreferredValue preferred_value);

PreferredValue floor_preferred_value(double numeric_value);
PreferredValue ceiling_preferred_value(double numeric_value);
PreferredValue nearest_preferred_value(double numeric_value);

void increment_preferred_value(PreferredValue *value);
void decrement_preferred_value(PreferredValue *value);

bool preferred_values_equal(PreferredValue value1, PreferredValue value2);
bool preferred_values_greater_than_or_equal(
    PreferredValue value1, PreferredValue value2
);
bool preferred_values_less_than_or_equal(
    PreferredValue value1, PreferredValue value2
);
#endif