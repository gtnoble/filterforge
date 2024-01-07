#ifndef FILTERFORGE_MEMORY
#define FILTERFORGE_MEMORY

#include <stdlib.h>

typedef struct {
    void *(*allocate)(size_t);
    void (*deallocate)(void *);
} MemoryManager;

#endif