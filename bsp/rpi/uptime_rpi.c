#include "bsp_uptime.h"

#include <time.h>

int64_t ah_getUptimeUs(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000000LL + now.tv_nsec / 1000;
}
