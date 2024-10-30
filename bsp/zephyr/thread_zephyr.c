#include "bsp_thread.h"

#include <zephyr/kernel.h>

#define THREAD_JOIN_TIMEOUT 100

typedef struct ah_sem_t
{
    struct k_sem hdl;
} ah_sem_t;

typedef struct ah_mutex_t
{
    struct k_mutex hdl;
} ah_mutex_t;

typedef struct ah_thread_t
{
    struct k_thread hdl;
    ah_threadFunc_t func;
    k_thread_stack_t *stack;
} ah_thread_t;

static k_timeout_t get_zephyr_timeout(int timeout_ms)
{
    if (timeout_ms == AH_NO_WAIT)
    {
        return K_NO_WAIT;
    }
    else if (timeout_ms == AH_NO_TIMEOUT)
    {
        return K_FOREVER;
    }
    else
    {
        return K_MSEC(timeout_ms);
    }
}

static void thread_function_wrapper(void *p1, void *, void *)
{
    ah_thread_t *thread = p1;
    thread->func();
}

ah_thread_t *ah_thread_create(ah_threadFunc_t entry, size_t stack_size, int prio, const char *name)
{
    ah_thread_t *thread = k_calloc(1, sizeof(ah_thread_t));
    if (!thread)
    {
        return NULL;
    }

    thread->stack = k_thread_stack_alloc(stack_size, 0);
    if (!thread->stack)
    {
        k_free(thread);
        return NULL;
    }

    thread->func = entry;
    k_thread_create(&thread->hdl, thread->stack, stack_size, thread_function_wrapper,
                    thread, NULL, NULL, prio, 0, get_zephyr_timeout(AH_NO_WAIT));

    if (name)
    {
        k_thread_name_set(&thread->hdl, name);
    }

    return thread;
}

void ah_thread_destroy(ah_thread_t *thread)
{
    if (thread)
    {
        k_thread_stack_free(thread->stack);
        k_free(thread);
    }
}

bool ah_thread_quit(ah_thread_t *thread)
{
    if (k_thread_join(&thread->hdl, get_zephyr_timeout(THREAD_JOIN_TIMEOUT)) != 0)
    {
        k_thread_abort(&thread->hdl);
    }

    return true;
}

ah_sem_t *ah_semaphore_create(unsigned int initial_count, unsigned int limit)
{
    ah_sem_t *sem = k_calloc(1, sizeof(ah_sem_t));
    if (!sem)
    {
        return NULL;
    }

    if (k_sem_init(&sem->hdl, initial_count, limit) == -EINVAL)
    {
        k_free(sem);
        return NULL;
    }

    return sem;
}

void ah_semaphore_destroy(ah_sem_t *sem)
{
    if (sem)
    {
        k_free(sem);
    }
}

bool ah_semaphore_take(ah_sem_t *sem, int timeout_ms)
{
    return k_sem_take(&sem->hdl, get_zephyr_timeout(timeout_ms)) == 0;
}

bool ah_semaphore_give(ah_sem_t *sem)
{
    k_sem_give(&sem->hdl);
    return true;
}

ah_mutex_t *ah_mutex_create(void)
{
    ah_mutex_t *mutex = k_calloc(1, sizeof(ah_mutex_t));
    if (!mutex)
    {
        return NULL;
    }

    if (k_mutex_init(&mutex->hdl) != 0)
    {
        k_free(mutex);
        return NULL;
    }

    return mutex;
}

void ah_mutex_destroy(ah_mutex_t *mutex)
{
    if (mutex)
    {
        k_free(mutex);
    }
}

bool ah_mutex_lock(ah_mutex_t *mutex, int timeout_ms)
{
    return k_mutex_lock(&mutex->hdl, get_zephyr_timeout(timeout_ms)) == 0;
}

bool ah_mutex_unlock(ah_mutex_t *mutex)
{
    return k_mutex_unlock(&mutex->hdl) == 0;
}
