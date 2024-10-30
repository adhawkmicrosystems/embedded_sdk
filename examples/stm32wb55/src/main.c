#include "example_common.h"
#include "stm32_init.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

static void mainTask(void *argument)
{
    (void)argument;

    printf("Starting AdHawk SDK example\n");

    adhawk_example_run();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

int main(void)
{
    Init_System();

    TaskHandle_t mainTaskHandle;
    xTaskCreate(mainTask, "MAIN TASK", 256 * 4, NULL, (configMAX_PRIORITIES - 3), &mainTaskHandle);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    while (1)
    {
    }
}
