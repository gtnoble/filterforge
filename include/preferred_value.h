#include <stdbool.h>

typedef struct {
    size_t index;
    int order_of_magnitude;
} PreferredValue;

PreferredValue *make_preferred_value(int value_index, int order_of_magnitude, void *allocate(size_t));
void free_preferred_value(PreferredValue *value, void deallocate(void *));
PreferredValue *duplicate_preferred_value(PreferredValue *preferred_value, void *allocate(size_t));
void copy_preferred_value(PreferredValue *source, PreferredValue *destination);
double evaluate_preferred_value(PreferredValue *preferred_value);

void floor_preferred_value(double numeric_value, PreferredValue *output);
void ceiling_preferred_value(double numeric_value, PreferredValue *output);
void nearest_preferred_value(double numeric_value, PreferredValue *output);
