#include "bsp_thread.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include <stdlib.h>

#if INCLUDE_vTaskSuspend != 1  // block semaphores indefinitely
#error "INCLUDE_vTaskSuspend is required!"
#endif

typedef struct ah_sem_t
{
    SemaphoreHandle_t hdl;
} ah_sem_t;

typedef struct ah_mutex_t
{
    SemaphoreHandle_t hdl;
} ah_mutex_t;

typedef struct ah_thread_t
{
    TaskHandle_t hdl;
    ah_threadFunc_t func;
} ah_thread_t;

static TickType_t get_freertos_timeout(int timeout_ms)
{
    if (timeout_ms == AH_NO_WAIT)
    {
        return 0;
    }
    else if (timeout_ms == AH_NO_TIMEOUT)
    {
        return portMAX_DELAY;
    }
    else
    {
        return pdMS_TO_TICKS(timeout_ms);
    }
}

static void thread_function_wrapper(void *pvParameters)
{
    ah_thread_t *thread = pvParameters;
    thread->func();
}

ah_thread_t *ah_thread_create(ah_threadFunc_t entry, size_t stack_size, int prio, const char *name)
{
    ah_thread_t *thread = calloc(1, sizeof(ah_thread_t));
    if (!thread)
    {
        return NULL;
    }

    thread->func = entry;

    // NOTE: esp32 FreeRTOS takes stack_size in bytes, and vanilla FreeRTOS in words.
    stack_size = stack_size / sizeof(StackType_t);

    // NOTE: xTaskCreate on esp32 creates an unpinned task that can run on either core
    // esp32 FreeRTOS scheduler implements a Best Effort Round Robin time slicing to account for multiple cores
    // To achieve ideal round-robin time slicing, users should ensure that all tasks of a particular priority are pinned to the same core.
    if (xTaskCreate(thread_function_wrapper, name, stack_size, thread, prio, &thread->hdl) != pdPASS)
    {
        free(thread);
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
    if (thread->hdl == NULL)
    {
        return false;
    }

    vTaskSuspend(thread->hdl);
    vTaskDelete(thread->hdl);
    return true;
}

ah_sem_t *ah_semaphore_create(unsigned initial_count, unsigned limit)
{
    ah_sem_t *sem = calloc(1, sizeof(ah_sem_t));
    if (!sem)
    {
        return NULL;
    }

    sem->hdl = xSemaphoreCreateCounting(limit, initial_count);
    if (!sem->hdl)
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
        vSemaphoreDelete(sem->hdl);
        free(sem);
    }
}

bool ah_semaphore_take(ah_sem_t *sem, int timeout_ms)
{
    return xSemaphoreTake(sem->hdl, get_freertos_timeout(timeout_ms)) == pdTRUE;
}

bool ah_semaphore_give(ah_sem_t *sem)
{
    return xSemaphoreGive(sem->hdl) == pdTRUE;
}

ah_mutex_t *ah_mutex_create(void)
{
    ah_mutex_t *mutex = calloc(1, sizeof(ah_mutex_t));
    if (!mutex)
    {
        return NULL;
    }

    mutex->hdl = xSemaphoreCreateMutex();
    if (!mutex->hdl)
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
        vSemaphoreDelete(mutex->hdl);
        free(mutex);
    }
}

bool ah_mutex_lock(ah_mutex_t *mutex, int timeout_ms)
{
    return xSemaphoreTake(mutex->hdl, get_freertos_timeout(timeout_ms)) == pdTRUE;
}

bool ah_mutex_unlock(ah_mutex_t *mutex)
{
    return xSemaphoreGive(mutex->hdl) == pdTRUE;
}
