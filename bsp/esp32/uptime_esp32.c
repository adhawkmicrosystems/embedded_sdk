#include "bsp_uptime.h"

#include <esp_timer.h>

int64_t ah_getUptimeUs(void)
{
    return esp_timer_get_time();
}
