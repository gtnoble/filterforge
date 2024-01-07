#ifndef FILTERFORGE_PREFERRED_VALUE
#define FILTERFORGE_PREFERRED_VALUE

#include <stdbool.h>
#include "memory.h"

typedef struct {
    size_t index;
    int order_of_magnitude;
    void (*deallocate)(void *);
} PreferredValue;

PreferredValue *make_preferred_value(int value_index, int order_of_magnitude, MemoryManager memory);
void free_preferred_value(PreferredValue *value);
PreferredValue *duplicate_preferred_value(const PreferredValue *preferred_value, MemoryManager memory);
void copy_preferred_value(const PreferredValue *source, PreferredValue *destination);
double evaluate_preferred_value(const PreferredValue *preferred_value);

PreferredValue floor_preferred_value(double numeric_value);
PreferredValue ceiling_preferred_value(double numeric_value);
PreferredValue nearest_preferred_value(double numeric_value);

void increment_preferred_value(PreferredValue *value);
void decrement_preferred_value(PreferredValue *value);

bool preferred_values_equal(const PreferredValue *value1, const PreferredValue *value2);
bool preferred_values_greater_than_or_equal(
    const PreferredValue *value1, const PreferredValue *value2
);
bool preferred_values_less_than_or_equal(
    const PreferredValue *value1, const PreferredValue *value2
);
#endif