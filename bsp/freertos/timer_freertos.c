#include "bsp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include <stdlib.h>

typedef struct ah_timer_t
{
    TimerHandle_t hdl;
    ah_timerCallback callback;
} ah_timer_t;

static void callback_wrapper(TimerHandle_t hdl)
{
    ah_timer_t *timer = (ah_timer_t *)pvTimerGetTimerID(hdl);
    timer->callback();
}

ah_timer_t *ah_timer_create(ah_timerCallback callback)
{
    ah_timer_t *timer = pvPortMalloc(sizeof(ah_timer_t));
    timer->callback = callback;

    // need to define a period on creation. this gets reset in ah_timer_startPeriodic
    timer->hdl = xTimerCreate("ah_timer", 100, pdTRUE, (void *)&timer->hdl, callback_wrapper);
    return timer;
}

void ah_timer_destroy(ah_timer_t *timer)
{
    xTimerDelete(timer->hdl, 0);
    vPortFree(timer);
}

void ah_timer_startPeriodic(ah_timer_t *timer, uint64_t period_us)
{
    // starts timer as well
    xTimerChangePeriod(timer->hdl, pdMS_TO_TICKS(period_us / 1000), 0);
}

void ah_timer_stop(ah_timer_t *timer)
{
    xTimerStop(timer->hdl, 0);
}
