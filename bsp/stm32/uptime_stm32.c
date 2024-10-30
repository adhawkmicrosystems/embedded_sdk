#include "bsp_uptime.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int64_t ah_getUptimeUs(void)
{
    // TODO: Use a system timer to improve resolution
    return xTaskGetTickCount() * (1000000 / configTICK_RATE_HZ);
}
