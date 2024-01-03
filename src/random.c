#include <libguile.h>
#include <stdbool.h>
#include "mtwister.h"

SCM make_prng(SCM seed);

void init_rng(void) {
    __extension__
    scm_c_define_gsubr("make-prng", 1, 0, 0, make_prng);
}

SCM make_prng(SCM seed) {
    MTRand *prng = scm_gc_malloc(sizeof(MTRand), "random number generator");
    *prng = seedRand(scm_to_ulong(seed));
    return scm_from_pointer(prng, NULL);
}

unsigned long gen_random(SCM prng) {
    return genRandLong(scm_to_pointer(prng));
}

bool gen_random_bool(SCM prng) {
    return gen_random(prng) & 1;
}