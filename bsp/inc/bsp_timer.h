#pragma once

#include <stdint.h>

typedef struct ah_timer_t ah_timer_t;

typedef void (*ah_timerCallback)(void);

//! Allocates and initializes a timer
ah_timer_t *ah_timer_create(ah_timerCallback callback);

//! Cleanup after a timer
void ah_timer_destroy(ah_timer_t *timer);

//! Starts the timer. Calling this on a running timer will restart the timer
void ah_timer_startPeriodic(ah_timer_t *timer, uint64_t period_us);

//! Stops a timer if it's running
void ah_timer_stop(ah_timer_t *timer);
