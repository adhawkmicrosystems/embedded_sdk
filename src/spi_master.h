#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    ah_spiPollingRate_Slow,  // Use when expected data rate is low to save power. Increases latency
    ah_spiPollingRate_Fast,  // Use when expected data rate is high
} ah_spiPollingRate;

typedef void (*ah_spiDataReadyCallback)(uint8_t isStream, const uint8_t *data, uint32_t len);

void ah_spi_master_init(ah_spiDataReadyCallback cb);

void ah_spi_master_deinit(void);

void ah_spi_master_setRate(ah_spiPollingRate rate);

bool ah_spi_master_send(const uint8_t *data, uint32_t len);
