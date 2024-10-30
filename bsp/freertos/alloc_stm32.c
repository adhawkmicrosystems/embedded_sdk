#include "bsp_alloc.h"

#include "freertos/FreeRTOS.h"

void ah_free(void *ptr)
{
    vPortFree(ptr);
}

void *ah_malloc(size_t size)
{
    return pvPortMalloc(size);
}
