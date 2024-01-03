#include <libguile.h>
#include <stdbool.h>
#include <math.h>

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

SCM preferred_component_value_type;

SCM floor_preferred_value(SCM numeric_value);
SCM ceiling_preferred_value(SCM numeric_value);
SCM nearest_preferred_value(SCM value_num);
SCM make_preferred_component_value(int value_index, int order_of_magnitude);
SCM scm_evaluated_component_value(SCM preferred_value);
SCM get_preferred_value_index(SCM preferred_value);
SCM get_preferred_value_order_of_magnitude(SCM preferred_value);

int get_preferred_component_value_index(SCM preferred_value);
int get_preferred_component_order_of_magnitude(SCM preferred_value);

void set_preferred_component_value_index(SCM preferred_value, int index);
void set_preferred_component_value_order_of_magnitude(SCM preferred_value, int order_of_magnitude);

double evaluated_component_value(SCM preferred_value);

bool component_values_greater_than(SCM value1, SCM value2);
bool component_values_less_than(SCM value1, SCM value2);
bool component_values_greater_than_or_equal(
    SCM value1, SCM value2
);
bool component_values_less_than_or_equal(
    SCM value1, SCM value2
);

void init_preferred_component_value_type(void) {
    SCM name, slots;
    scm_t_struct_finalize finalizer;

    name = scm_from_utf8_symbol("component-value");
    slots = scm_list_2(
        scm_from_utf8_symbol("value-index"), 
        scm_from_utf8_symbol("order-of-magnitude")
    );
    finalizer = NULL;
    preferred_component_value_type = scm_make_foreign_object_type(name, slots, finalizer);

    __extension__
    scm_c_define_gsubr("ceiling-preferred-value", 1, 0, 0, (scm_t_subr) ceiling_preferred_value);
    __extension__
    scm_c_define_gsubr("floor-preferred-value", 1, 0, 0, (scm_t_subr) floor_preferred_value);
    __extension__
    scm_c_define_gsubr("nearest-preferred-value", 1, 0, 0, (scm_t_subr) nearest_preferred_value);
    __extension__
    scm_c_define_gsubr("increment-preferred-value", 1, 0, 0, (scm_t_subr) increment_component_value);
    __extension__
    scm_c_define_gsubr("decrement-preferred-value", 1, 0, 0, (scm_t_subr) decrement_component_value);
    __extension__
    scm_c_define_gsubr("evaluate-preferred-value", 1, 0, 0, (scm_t_subr) scm_evaluated_component_value);
    __extension__
    scm_c_define_gsubr("get-preferred-value-index", 1, 0, 0, (scm_t_subr) get_preferred_value_index);
    __extension__
    scm_c_define_gsubr("get-preferred-value-order-of-magnitude", 1, 0, 0, (scm_t_subr) get_preferred_value_order_of_magnitude);
}

SCM make_preferred_component_value(int value_index, int order_of_magnitude) {
    int *value_indexp = scm_gc_malloc(sizeof(int), "value index");
    *value_indexp = value_index;
    int *order_of_magnitudep = scm_gc_malloc(sizeof(int), "order_of_magnitude");
    *order_of_magnitudep = order_of_magnitude;
    return scm_make_foreign_object_2(
        preferred_component_value_type, 
        value_indexp, 
        order_of_magnitudep
    );
}

SCM duplicate_preferred_component_value(SCM preferred_value) {
    return make_preferred_component_value(
        get_preferred_component_value_index(preferred_value),
        get_preferred_component_order_of_magnitude(preferred_value)
    );
}

int get_preferred_component_value_index(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int *indexp = scm_foreign_object_ref(preferred_value, 0);
    return *indexp;
}

SCM get_preferred_value_index(SCM preferred_value) {
    return scm_from_int(get_preferred_component_value_index(preferred_value));
}

void set_preferred_component_value_index(SCM preferred_value, int index) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int *indexp = scm_foreign_object_ref(preferred_value, 0);
    *indexp = index;
}

int get_preferred_component_order_of_magnitude(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int *order_of_magnitudep = scm_foreign_object_ref(preferred_value, 1);
    return *order_of_magnitudep;
}

SCM get_preferred_value_order_of_magnitude(SCM preferred_value) {
    return scm_from_int(get_preferred_component_order_of_magnitude(preferred_value));
}

void set_preferred_component_value_order_of_magnitude(SCM preferred_value, int order_of_magnitude) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int *order_of_magnitudep = scm_foreign_object_ref(preferred_value, 1);
    *order_of_magnitudep = order_of_magnitude;
}

double evaluated_component_value(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int value_index = get_preferred_component_value_index(preferred_value);
    int order_of_magnitude = get_preferred_component_order_of_magnitude(preferred_value);
    return pow(10.0, order_of_magnitude) * e24_values[value_index];
}

SCM scm_evaluated_component_value(SCM preferred_value) {
    return scm_from_double(evaluated_component_value(preferred_value));
}

SCM floor_preferred_value(SCM numeric_value) {

    double log_value = log10(scm_to_double(numeric_value));
    int order_of_magnitude = (int) floor(log_value);
    int eseries_index;
    for (int i = 0; i < num_e24_values; i++) {
        if (log_value - order_of_magnitude >= log10(e24_values[i])) {
            eseries_index = i;
        }
    }

    return make_preferred_component_value(
        eseries_index,
        order_of_magnitude
    );
}

SCM ceiling_preferred_value(SCM numeric_value) {

    double log_value = log10(scm_to_double(numeric_value));
    int order_of_magnitude = (int) floor(log_value);
    int eseries_index;
    for (int i = num_e24_values - 1; i >= 0; i--) {
        if (log_value - order_of_magnitude <= log10(e24_values[i])) {
            eseries_index = i;
        }
    }

    return make_preferred_component_value(
        eseries_index,
        order_of_magnitude
    );
}

SCM nearest_preferred_value(SCM value_num) {

    SCM floor = floor_preferred_value(value_num);
    SCM ceiling = ceiling_preferred_value(value_num);

    if (
        fabs(evaluated_component_value(floor) - scm_to_double(value_num)) <= 
        fabs(evaluated_component_value(ceiling) - scm_to_double(value_num))
    ) {
        return floor;
    }
    else {
        return ceiling;
    }
}
SCM increment_component_value(SCM value) {
    int current_value_index = get_preferred_component_value_index(value);
    int current_order_of_magnitude = get_preferred_component_order_of_magnitude(value);

    int next_value_index;
    int next_order_of_magnitude;

    if (current_value_index == num_e24_values - 1) {
        next_value_index = 0;
        next_order_of_magnitude = current_order_of_magnitude + 1;
    }
    else {
        next_value_index = current_value_index + 1;
        next_order_of_magnitude = current_order_of_magnitude;
    }

    set_preferred_component_value_index(value, next_value_index);
    set_preferred_component_value_order_of_magnitude(value, next_order_of_magnitude);
    return value;
}

SCM decrement_component_value(SCM value) {
    int current_value_index = get_preferred_component_value_index(value);
    int current_order_of_magnitude = get_preferred_component_order_of_magnitude(value);

    int next_value_index;
    int next_order_of_magnitude;

    if (current_value_index == 0) {
        next_value_index = num_e24_values - 1;
        next_order_of_magnitude = current_order_of_magnitude - 1;
    }
    else {
        next_value_index = current_value_index - 1;
        next_order_of_magnitude = current_order_of_magnitude;
    }

    set_preferred_component_value_index(value, next_value_index);
    set_preferred_component_value_order_of_magnitude(value, next_order_of_magnitude);
    return value;
}

bool component_values_equal(SCM value1, SCM value2) {
    return 
        get_preferred_component_value_index(value1) == get_preferred_component_value_index(value2) && 
        get_preferred_component_order_of_magnitude(value1) == get_preferred_component_order_of_magnitude(value2);
}

bool component_values_less_than(SCM value1, SCM value2) {
    if (get_preferred_component_order_of_magnitude(value1) < get_preferred_component_order_of_magnitude(value2)) {
        return true;
    }
    else if (
        get_preferred_component_order_of_magnitude(value1) == get_preferred_component_order_of_magnitude(value2) && 
        get_preferred_component_value_index(value1) < get_preferred_component_value_index(value2)
    ) {
        return true;
    }
    return false;
}

bool component_values_greater_than(SCM value1, SCM value2) {
    if (get_preferred_component_order_of_magnitude(value1) > get_preferred_component_order_of_magnitude(value2)) {
        return true;
    }
    else if (
        get_preferred_component_order_of_magnitude(value1) == get_preferred_component_order_of_magnitude(value2) && 
        get_preferred_component_value_index(value1) > get_preferred_component_value_index(value2)
    ) {
        return true;
    }
    return false;

}

bool component_values_greater_than_or_equal(
    SCM value1, SCM value2
) {
    return 
        component_values_equal(value1, value2) || 
        component_values_greater_than(value1, value2);
}

bool component_values_less_than_or_equal(
    SCM value1, SCM value2
) {
    return 
        component_values_equal(value1, value2) || 
        component_values_less_than(value1, value2);
}

