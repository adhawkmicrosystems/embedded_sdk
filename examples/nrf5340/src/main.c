#include "example_common.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

int main(void)
{
    LOG_INF("Starting AdHawk SDK example");

    adhawk_example_run();

    while (true)
    {
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
