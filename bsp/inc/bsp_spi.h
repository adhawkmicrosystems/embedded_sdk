#pragma once

#include <stdbool.h>
#include <stdint.h>

#define AH_SPI_SOF_BYTE (0xAA)
#define AH_SPI_FRAME_SIZE (1024)
#define AH_SPI_HEADER_SIZE (3)

//! The maximum supported frequency is 27.5 MHz
#define AH_SPI_FREQUENCY (27500000)

//! Initializes any SPI hardware objects
bool ah_spi_init(void);

//! Cleanup after SPI
void ah_spi_deinit(void);

//! Get the SPI thread priority for the platform
int ah_spi_getThreadPriority(void);

//! Performs a single full duplex transaction
void ah_spi_transferFullDuplex(uint8_t txFrame[AH_SPI_FRAME_SIZE], uint8_t rxFrame[AH_SPI_FRAME_SIZE]);
