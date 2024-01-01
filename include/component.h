#ifndef FILTOPT_COMPONENT
#define FILTOPT_COMPONENT

#include <complex.h>
#include <libguile.h>

extern SCM component_type;

void init_component_type(void);
double complex component_impedance(double angular_frequency, SCM component);

#endif
