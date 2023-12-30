#include <libguile.h>
#include <stdbool.h>

#include <preferred_value.h>

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

const double log10_e24_values[] = {
    0, 0.04139268515822507, 0.07918124604762482, 0.11394335230683678, 
    0.17609125905568124, 0.2041199826559248, 0.25527250510330607, 
    0.3010299956639812, 0.3424226808222063, 0.38021124171160603, 
    0.43136376415898736, 0.47712125471966244, 0.5185139398778874, 
    0.5563025007672873, 0.5910646070264992, 0.6334684555795865, 
    0.6720978579357175, 0.7075701760979364, 0.7481880270062004, 
    0.7923916894982539, 0.8325089127062363, 0.8750612633917001, 
    0.9138138523837167, 0.9590413923210935
};

const int num_e24_values = 24;

SCM preferred_component_value_type;



SCM floor_preferred_value(double value);
SCM ceiling_preferred_value(double value);
SCM nearest_preferred_value(SCM value_num);
SCM increment_component_value(SCM value); 
SCM decrement_component_value(SCM value);
SCM random_update(
    SCM value, SCM min_range, SCM max_range
);

bool component_values_equal(SCM value1, SCM value2);
bool component_values_less_than(SCM value1, SCM value2);
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

    name = scm_list_1(scm_from_utf8_symbol("component-value"));
    slots = scm_list_2(
        scm_from_utf8_symbol("value-index"), 
        scm_from_utf8_symbol("order-of-magnitude")
    );
    finalizer = NULL;
    preferred_component_value_type = scm_make_foreign_object_type(name, slots, finalizer);

    scm_c_define_gsubr("nearest-preferred-value", 1, 0, 0, nearest_preferred_value);
    scm_c_define_gsubr("increment-component-value", 1, 0, 0, increment_component_value);
    scm_c_define_gsubr("decrement-component-value", 1, 0, 0, decrement_component_value);
    scm_c_define_gsubr("random-compenent-value-update", 3, 0, 0, random_update);
}

SCM make_preferred_component_value(int value_index, int order_of_magnitude) {
    return scm_make_foreign_object_2(
        preferred_component_value_type, 
        value_index, 
        order_of_magnitude
    );
}

int get_preferred_component_value_index(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    return scm_foreign_object_ref(preferred_value, 0);
}

void set_preferred_component_value_index(SCM preferred_value, int index) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    scm_foreign_object_set_x(preferred_value, 0, index);
}

int get_preferred_component_order_of_magnitude(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    return scm_foreign_object_ref(preferred_value, 1);
}

void set_preferred_component_value_order_of_magnitude(SCM preferred_value, int order_of_magnitude) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    scm_foreign_object_set_x(preferred_value, 1, order_of_magnitude);
}

double evaluated_component_value(SCM preferred_value) {
    scm_assert_foreign_object_type(preferred_component_value_type, preferred_value);
    int value_index = get_preferred_component_value_index(preferred_value);
    int order_of_magnitude = get_preferred_component_order_of_magnitude(preferred_value);
    return pow(
        10.0, 
        order_of_magnitude + log10_e24_values[value_index]
    );
}

SCM floor_preferred_value(double value) {

    double log_value = log10(value);
    double order_of_magnitude = floor(log_value);
    int eseries_index;
    for (int i = 0; i < num_e24_values; i++) {
        if (log_value - order_of_magnitude >= log10_e24_values[i]) {
            eseries_index = i;
        }
    }

    return make_preferred_component_value(
        (int) order_of_magnitude,
        eseries_index
    );
}

SCM ceiling_preferred_value(double value) {

    double log_value = log10(value);
    double order_of_magnitude = floor(log_value);
    int eseries_index;
    for (int i = num_e24_values - 1; i >= 0; i--) {
        if (log_value - order_of_magnitude <= log10_e24_values[i]) {
            eseries_index = i;
        }
    }

    return make_preferred_component_value((int) order_of_magnitude, eseries_index);
}

SCM nearest_preferred_value(SCM value_num) {

    double value = scm_to_double(value_num);
    SCM floor = floor_preferred_value(value);
    SCM ceiling = ceiling_preferred_value(value);

    if (fabs(evaluated_component_value(floor) - value) <= fabs(evaluated_component_value(ceiling) - value)) {
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
        next_order_of_magnitude = next_order_of_magnitude + 1;
    }
    else {
        next_value_index = current_value_index + 1;
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

SCM random_update(
    SCM value, SCM min_range, SCM max_range
) {
    scm_assert_foreign_object_type(preferred_component_value_type, value);
    scm_assert_foreign_object_type(preferred_component_value_type, min_range);
    scm_assert_foreign_object_type(preferred_component_value_type, max_range);


    if (component_values_equal(value, min_range)) {
        return increment_component_value(value);
    }
    else if (component_values_equal(value, max_range)) {
        return decrement_component_value(value);
    }
    else {
        if(rand() < RAND_MAX / 2) {
            decrement_component_value(value);
        } 
        else {
            increment_component_value(value);
        }
    }
    return value;
}
