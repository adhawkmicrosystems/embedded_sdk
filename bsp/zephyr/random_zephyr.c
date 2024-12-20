#include "bsp_random.h"

#include <zephyr/random/random.h>

void ah_random(void *data, unsigned len)
{
    sys_rand_get(data, len);
}
