#include "bsp_alloc.h"

#include <stddef.h>
#include <stdlib.h>

__attribute__((weak)) void ah_free(void *ptr)
{
    free(ptr);
}

__attribute__((weak)) void *ah_malloc(size_t size)
{
    return malloc(size);
}

__attribute__((weak)) void *ah_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

__attribute__((weak)) void *ah_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}
