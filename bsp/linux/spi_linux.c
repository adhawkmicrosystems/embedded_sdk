#include "ah_spi_pins.h"
#include "bsp_spi.h"

#include <fcntl.h>
#include <gpiod.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SPI_THREAD_PRIORITY 0

const int SPI_MODE = 0;  // idle low, capture at rising
const int SPI_BITS_PER_WORD = 8;
const int SPI_FREQUENCY = AH_SPI_FREQUENCY;

static int s_device = -1;
static struct gpiod_line_request *s_gpioRequest;

static void set_cs_gpio(enum gpiod_line_value value)
{
    const unsigned offset = AH_SPI_CS_GPIO_NUM;
    gpiod_line_request_set_value(s_gpioRequest, offset, value);
}

static void init_cs_gpio(void)
{
    struct gpiod_chip *chip = gpiod_chip_open(AH_SPI_CS_GPIO_CHIP);
    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    const unsigned offset = AH_SPI_CS_GPIO_NUM;
    gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);

    s_gpioRequest = gpiod_chip_request_lines(chip, NULL, line_cfg);

    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(settings);
    gpiod_chip_close(chip);
}

bool ah_spi_init(void)
{
    s_device = open(AH_SPI_HOST, O_RDWR);
    if (s_device == -1)
    {
        printf("Failed to open %s\n", AH_SPI_HOST);
        return false;
    }

    ioctl(s_device, SPI_IOC_WR_MODE, &SPI_MODE);
    ioctl(s_device, SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS_PER_WORD);
    ioctl(s_device, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_FREQUENCY);

    init_cs_gpio();

    return true;
}

void ah_spi_deinit(void)
{
    if (s_device != -1)
    {
        close(s_device);
        s_device = -1;
    }

    gpiod_line_request_release(s_gpioRequest);
}

int ah_spi_getThreadPriority(void)
{
    return SPI_THREAD_PRIORITY;
}

void ah_spi_transferFullDuplex(uint8_t txFrame[AH_SPI_FRAME_SIZE], uint8_t rxFrame[AH_SPI_FRAME_SIZE])
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txFrame,
        .rx_buf = (unsigned long)rxFrame,
        .len = AH_SPI_FRAME_SIZE,
        .delay_usecs = 0,
        .speed_hz = SPI_FREQUENCY,
        .bits_per_word = SPI_BITS_PER_WORD,
    };

    set_cs_gpio(GPIOD_LINE_VALUE_INACTIVE);
    ioctl(s_device, SPI_IOC_MESSAGE(1), &tr);
    set_cs_gpio(GPIOD_LINE_VALUE_ACTIVE);
}
