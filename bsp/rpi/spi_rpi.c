#include "ah_spi_pins.h"
#include "bsp_spi.h"
#include "gpio_rpi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SPI_THREAD_PRIORITY 0

const int SPI_MODE = 0;  // idle low, capture at rising
const int SPI_BITS_PER_WORD = 8;
const int SPI_FREQUENCY = AH_SPI_FREQUENCY;

static int s_device = -1;

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

    ah_gpioInit();
    ah_gpioSetDirection(AH_SPI_PIN_CS, GPIO_DIR_OUTPUT);
    ah_gpioWritePin(AH_SPI_PIN_CS, GPIO_PIN_SET);

    return true;
}

void ah_spi_deinit(void)
{
    if (s_device != -1)
    {
        close(s_device);
    }
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

    ah_gpioWritePin(AH_SPI_PIN_CS, GPIO_PIN_RESET);
    ioctl(s_device, SPI_IOC_MESSAGE(1), &tr);
    fsync(s_device);
    ah_gpioWritePin(AH_SPI_PIN_CS, GPIO_PIN_SET);
}
