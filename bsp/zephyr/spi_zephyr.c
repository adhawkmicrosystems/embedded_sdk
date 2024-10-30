#include "bsp_spi.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>

#define SPI_THREAD_PRIORITY 2

#define SPI_DEV_NODE DT_NODELABEL(spi4)

static const struct device *s_device;

static const struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .frequency = AH_SPI_FREQUENCY,
    .slave = 0,
    .cs = {
        .gpio = GPIO_DT_SPEC_GET(SPI_DEV_NODE, cs_gpios),
        .delay = 0,
    },
};

bool ah_spi_init(void)
{
    s_device = DEVICE_DT_GET(SPI_DEV_NODE);

    if (!device_is_ready(s_device))
    {
        return false;
    }

    if (!device_is_ready(spi_cfg.cs.gpio.port))
    {
        return false;
    }

    return true;
}

void ah_spi_deinit(void)
{
    if (s_device)
    {
        s_device = NULL;
    }
}

int ah_spi_getThreadPriority(void)
{
    return SPI_THREAD_PRIORITY;
}

void ah_spi_transferFullDuplex(uint8_t txFrame[AH_SPI_FRAME_SIZE], uint8_t rxFrame[AH_SPI_FRAME_SIZE])
{
    struct spi_buf tx_buf = {
        .buf = txFrame,
        .len = AH_SPI_FRAME_SIZE
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    struct spi_buf rx_buf = {
        .buf = rxFrame,
        .len = AH_SPI_FRAME_SIZE
    };
    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1
    };

    spi_transceive(s_device, &spi_cfg, &tx, &rx);
}
