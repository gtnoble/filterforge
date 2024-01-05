#include <stdbool.h>
#include <stdlib.h>
#include "mtwister.h"

MTRand *make_prng(unsigned long seed, void *allocate(size_t)) {
    MTRand *prng = allocate(sizeof(MTRand));
    *prng = seedRand(seed);
    return prng;
}

void free_prng(MTRand *prng, void deallocate(void *)) {
    deallocate(prng);
}

unsigned long gen_random(MTRand *prng) {
    return genRandLong(prng);
}

bool gen_random_bool(MTRand *prng) {
    return gen_random(prng) & 1;
}