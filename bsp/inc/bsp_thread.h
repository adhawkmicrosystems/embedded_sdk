#pragma once

#include <stdbool.h>
#include <stddef.h>

#define AH_NO_TIMEOUT -1
#define AH_NO_WAIT 0

typedef struct ah_sem_t ah_sem_t;
typedef struct ah_mutex_t ah_mutex_t;
typedef struct ah_thread_t ah_thread_t;

typedef void (*ah_threadFunc_t)(void);

//! Allocates and runs a thread. The thread's stack will be allocated dynamically
ah_thread_t *ah_thread_create(ah_threadFunc_t entry, size_t stack_size, int prio, const char *name);
//! Terminate a running thread
bool ah_thread_quit(ah_thread_t *thread);
//! Cleanup after a thread
void ah_thread_destroy(ah_thread_t *thread);

//! Allocates and initializes a semaphore
ah_sem_t *ah_semaphore_create(unsigned initial_count, unsigned limit);
//! Cleanup after a semaphore
void ah_semaphore_destroy(ah_sem_t *sem);
//! Take a semaphore. Waits up to timeout_ms, or one of the special values AH_NO_WAIT / AH_NO_TIMEOUT
bool ah_semaphore_take(ah_sem_t *sem, int timeout_ms);
//! Give a semaphore
bool ah_semaphore_give(ah_sem_t *sem);

//! Allocates and initializes a mutex
ah_mutex_t *ah_mutex_create(void);
//! Cleanup after a mutex
void ah_mutex_destroy(ah_mutex_t *mutex);
//! Lock a mutex. Waits up to timeout_ms, or one of the special values AH_NO_WAIT / AH_NO_TIMEOUT
bool ah_mutex_lock(ah_mutex_t *mutex, int timeout_ms);
//! Unlocks a mutex
bool ah_mutex_unlock(ah_mutex_t *mutex);
