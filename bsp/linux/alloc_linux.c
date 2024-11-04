#include "bsp_alloc.h"

#include "stdlib.h"

void ah_free(void *ptr)
{
    free(ptr);
}

void *ah_malloc(size_t size)
{
    return malloc(size);
}

void *ah_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}
