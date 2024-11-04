#include "bsp_thread.h"

#include <pthread.h>
#include <semaphore.h>

#include <stdlib.h>

typedef struct ah_thread_t
{
    pthread_t hdl;
    ah_threadFunc_t func;
} ah_thread_t;

typedef struct ah_mutex_t
{
    pthread_mutex_t hdl;
} ah_mutex_t;

typedef struct ah_sem_t
{
    sem_t hdl;
} ah_sem_t;

static void *thread_function_wrapper(void *arg)
{
    ah_thread_t *thread = arg;
    thread->func();
    return NULL;
}

ah_thread_t *ah_thread_create(ah_threadFunc_t entry, size_t stack_size, int prio, const char *name)
{
    ah_thread_t *thread = calloc(1, sizeof(ah_thread_t));
    if (!thread)
    {
        return NULL;
    }

    thread->func = entry;
    if (pthread_create(&thread->hdl, NULL, thread_function_wrapper, thread) != 0)
    {
        return NULL;
    }

    return thread;
}

void ah_thread_destroy(ah_thread_t *thread)
{
    if (thread)
    {
        free(thread);
    }
}

bool ah_thread_quit(ah_thread_t *thread)
{
    return pthread_join(thread->hdl, NULL) == 0;
}

ah_sem_t *ah_semaphore_create(unsigned initial_count, unsigned limit)
{
    ah_sem_t *sem = calloc(1, sizeof(ah_sem_t));
    if (!sem)
    {
        return NULL;
    }

    if (sem_init(&sem->hdl, 0, initial_count) != 0)
    {
        free(sem);
        return NULL;
    }

    return sem;
}

void ah_semaphore_destroy(ah_sem_t *sem)
{
    if (sem)
    {
        sem_destroy(&sem->hdl);
        free(sem);
    }
}

bool ah_semaphore_take(ah_sem_t *sem, int timeout_ms)
{
    if (timeout_ms == AH_NO_TIMEOUT)
    {
        return sem_wait(&sem->hdl) == 0;
    }
    else if (timeout_ms == AH_NO_WAIT)
    {
        return sem_trywait(&sem->hdl) == 0;
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        return sem_timedwait(&sem->hdl, &ts) == 0;
    }
}

bool ah_semaphore_give(ah_sem_t *sem)
{
    return sem_post(&sem->hdl) == 0;
}

ah_mutex_t *ah_mutex_create(void)
{
    ah_mutex_t *mutex = calloc(1, sizeof(ah_mutex_t));
    if (!mutex)
    {
        return NULL;
    }

    if (pthread_mutex_init(&mutex->hdl, NULL) != 0)
    {
        free(mutex);
        return NULL;
    }

    return mutex;
}

void ah_mutex_destroy(ah_mutex_t *mutex)
{
    if (mutex)
    {
        pthread_mutex_destroy(&mutex->hdl);
        free(mutex);
    }
}

bool ah_mutex_lock(ah_mutex_t *mutex, int timeout_ms)
{
    if (timeout_ms == AH_NO_TIMEOUT)
    {
        return pthread_mutex_lock(&mutex->hdl) == 0;
    }
    else if (timeout_ms == AH_NO_WAIT)
    {
        return pthread_mutex_trylock(&mutex->hdl) == 0;
    }
    else
    {
        return false;
    }
}

bool ah_mutex_unlock(ah_mutex_t *mutex)
{
    return pthread_mutex_unlock(&mutex->hdl) == 0;
}
