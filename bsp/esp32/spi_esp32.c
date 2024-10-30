#include "ah_spi_pins.h"
#include "bsp_spi.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"

#define SPI_BITS_PER_WORD 8
#define SPI_THREAD_PRIORITY (configMAX_PRIORITIES - 2)

static spi_device_handle_t s_device;

bool ah_spi_init(void)
{
    esp_err_t rv;

    spi_bus_config_t buscfg = {
        .miso_io_num = AH_SPI_PIN_MISO,
        .mosi_io_num = AH_SPI_PIN_MOSI,
        .sclk_io_num = AH_SPI_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = AH_SPI_FRAME_SIZE,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = AH_SPI_FREQUENCY,
        .spics_io_num = AH_SPI_PIN_CS,
        .mode = 0,
        .queue_size = 1,
    };

    rv = spi_bus_initialize(AH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(rv);

    rv = spi_bus_add_device(AH_SPI_HOST, &devcfg, &s_device);
    ESP_ERROR_CHECK(rv);

    return true;
}

void ah_spi_deinit(void)
{
    s_device = NULL;
}

int ah_spi_getThreadPriority(void)
{
    return SPI_THREAD_PRIORITY;
}

void ah_spi_transferFullDuplex(uint8_t txFrame[AH_SPI_FRAME_SIZE], uint8_t rxFrame[AH_SPI_FRAME_SIZE])
{
    spi_transaction_t t = {
        .tx_buffer = txFrame,
        .rx_buffer = rxFrame,
        .length = AH_SPI_FRAME_SIZE * SPI_BITS_PER_WORD,
    };

    spi_device_transmit(s_device, &t);
}
