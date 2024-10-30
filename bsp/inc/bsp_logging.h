#pragma once

#ifdef ESP_PLATFORM

#include <esp_log.h>
#define AH_LOG_STR "ah_sdk"
#define ah_log_error(msg, ...) ESP_LOGE(AH_LOG_STR, msg, ##__VA_ARGS__)
#define ah_log_warn(msg, ...) ESP_LOGW(AH_LOG_STR, msg, ##__VA_ARGS__)
#define ah_log_info(msg, ...) ESP_LOGI(AH_LOG_STR, msg, ##__VA_ARGS__)
#define ah_log_debug(msg, ...) ESP_LOGD(AH_LOG_STR, msg, ##__VA_ARGS__)

#elif __ZEPHYR__

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(ah_sdk);
#define ah_log_error(msg, ...) LOG_ERR(msg, ##__VA_ARGS__)
#define ah_log_warn(msg, ...) LOG_WRN(msg, ##__VA_ARGS__)
#define ah_log_info(msg, ...) LOG_INF(msg, ##__VA_ARGS__)
#define ah_log_debug(msg, ...) LOG_DBG(msg, ##__VA_ARGS__)

#elif STM32_UART_LOGGING

#include "stm32_init.h"
#include "stm32wbxx_hal.h"
#include <stdio.h>
#define ah_log_error(msg, ...) printf(msg "\n\r", ##__VA_ARGS__);
#define ah_log_warn(msg, ...) printf(msg "\n\r", ##__VA_ARGS__);
#define ah_log_info(msg, ...) printf(msg "\n\r", ##__VA_ARGS__);
#define ah_log_debug(msg, ...) printf(msg "\n\r", ##__VA_ARGS__);

#elif __linux__

#include <stdio.h>
#define ah_log_error(msg, ...) printf(msg "\n", ##__VA_ARGS__);
#define ah_log_warn(msg, ...) printf(msg "\n", ##__VA_ARGS__);
#define ah_log_info(msg, ...) printf(msg "\n", ##__VA_ARGS__);
#define ah_log_debug(msg, ...) printf(msg "\n", ##__VA_ARGS__);

#else

#define ah_log_error(msg, ...)
#define ah_log_warn(msg, ...)
#define ah_log_info(msg, ...)
#define ah_log_debug(msg, ...)

#endif
