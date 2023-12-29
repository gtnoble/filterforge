#ifndef FILTOPT_COMPONENT
#define FILTOPT_COMPONENT

#include <complex.h>
#include <libguile.h>

extern SCM component_type;


double evaluated_component_value(SCM preferred_value);
double complex component_impedance(double angular_frequency, SCM component);
void random_component_update(SCM component);

#endif
