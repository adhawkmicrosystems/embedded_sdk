#include "bsp_alloc.h"

#include <zephyr/kernel.h>

void ah_free(void *ptr)
{
    k_free(ptr);
}

void *ah_malloc(size_t size)
{
    return k_malloc(size);
}

void *ah_calloc(size_t nmemb, size_t size)
{
    return k_calloc(nmemb, size);
}

/*
void *ah_realloc(void *ptr, size_t size)
{
    // The current nRF SDK version uses Zephyr v3.6, realloc should be avialble in v3.7
    // https://github.com/zephyrproject-rtos/zephyr/pull/72483
    return k_realloc(ptr, size);
}
*/
