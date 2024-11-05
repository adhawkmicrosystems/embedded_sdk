#include "bsp_timer.h"

#include <signal.h>
#include <stdlib.h>
#include <time.h>

typedef struct ah_timer_t
{
    timer_t hdl;
    ah_timerCallback callback;
} ah_timer_t;

static void callback_wrapper(union sigval val)
{
    ah_timer_t *timer = (ah_timer_t *)val.sival_ptr;
    timer->callback();
}

ah_timer_t *ah_timer_create(ah_timerCallback callback)
{
    if (!callback)
    {
        return NULL;
    }

    ah_timer_t *timer = malloc(sizeof(ah_timer_t));
    if (timer)
    {
        struct sigevent sev = {
            .sigev_notify = SIGEV_THREAD,
            .sigev_notify_function = callback_wrapper,
            .sigev_value.sival_ptr = timer,
        };

        timer_create(CLOCK_MONOTONIC, &sev, &timer->hdl);

        timer->callback = callback;
    }

    return timer;
}

void ah_timer_destroy(ah_timer_t *timer)
{
    timer_delete(timer->hdl);
    free(timer);
}

void ah_timer_startPeriodic(ah_timer_t *timer, uint64_t period_us)
{
    struct itimerspec its = {
        .it_value.tv_sec = period_us / 1000000,
        .it_value.tv_nsec = (period_us % 1000000) * 1000,
    };
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(timer->hdl, 0, &its, NULL);
}

void ah_timer_stop(ah_timer_t *timer)
{
    timer_delete(timer->hdl);
}
