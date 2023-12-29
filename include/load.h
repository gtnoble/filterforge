#ifndef FILTOPT_LOAD
#define FILTOPT_LOAD

#include <complex.h>
#include <libguile.h>
#include "component.h"

extern SCM load_type;

double complex load_impedance(double angular_frequency, SCM load);
double complex admittance(double angular_frequency, SCM load);

#endif
