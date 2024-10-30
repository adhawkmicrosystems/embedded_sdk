#include "bsp_uptime.h"

#include <zephyr/kernel.h>

int64_t ah_getUptimeUs(void)
{
    return k_uptime_ticks() * USEC_PER_SEC / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
}
