#pragma once

#include <stddef.h>

void ah_free(void *ptr);

void *ah_malloc(size_t size);

void *ah_calloc(size_t nmemb, size_t size);

void *ah_realloc(void *ptr, size_t size);