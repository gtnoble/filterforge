#ifndef FILTOPT_FILTER
#define FILTOPT_FILTER

#include <libguile.h>

#include "load.h"
#include "two_port_network.h"

void filter_stage_network(TwoPortNetwork *network, double angular_frequency, SCM stage);
void filter_network(TwoPortNetwork *network, double angular_frequency, SCM stages);

#endif
