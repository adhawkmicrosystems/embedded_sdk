#include "bsp_timer.h"

#include <zephyr/kernel.h>

typedef struct ah_timer_t
{
    struct k_timer hdl;
    ah_timerCallback callback;
} ah_timer_t;

static void callback_wrapper(struct k_timer *hdl)
{
    ah_timer_t *timer = (ah_timer_t *)hdl;
    timer->callback();
}

ah_timer_t *ah_timer_create(ah_timerCallback callback)
{
    ah_timer_t *timer = k_malloc(sizeof(ah_timer_t));
    timer->callback = callback;
    k_timer_init(&timer->hdl, callback_wrapper, NULL);
    return timer;
}

void ah_timer_destroy(ah_timer_t *timer)
{
    k_timer_stop(&timer->hdl);
    k_free(timer);
}

void ah_timer_startPeriodic(ah_timer_t *timer, uint64_t period_us)
{
    k_timer_start(&timer->hdl, K_USEC(period_us), K_USEC(period_us));
}

void ah_timer_stop(ah_timer_t *timer)
{
    k_timer_stop(&timer->hdl);
}
