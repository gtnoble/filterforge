#include <stdbool.h>
#include <stdlib.h>
#include "mtwister.h"

MTRand *make_prng(unsigned long seed) {
    MTRand *prng = malloc(sizeof(MTRand));
    *prng = seedRand(seed);
    return prng;
}

void free_prng(MTRand *prng) {
    free(prng);
}

unsigned long gen_random(MTRand *prng) {
    return genRandLong(prng);
}

bool gen_random_bool(MTRand *prng) {
    return gen_random(prng) & 1;
}