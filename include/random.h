#ifndef FILTERFORGE_RANDOM
#define FILTERFORGE_RANDOM

#include "mtwister.h"

unsigned long gen_random(MTRand *prng);
bool gen_random_bool(MTRand *prng);

#endif