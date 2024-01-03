#include <libguile.h>

void init_rng(void);
unsigned long gen_random(SCM prng);