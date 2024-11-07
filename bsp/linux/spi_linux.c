#include "ah_spi_pins.h"
#include "bsp_spi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SPI_THREAD_PRIORITY 0

const int SPI_MODE = 0;  // idle low, capture at rising
const int SPI_BITS_PER_WORD = 8;
static int SPI_FREQUENCY = AH_SPI_FREQUENCY;

static int s_device = -1;

bool ah_spi_init(void)
{
    char *spidev = getenv("AH_SPI_DEV");
    if (!spidev)
    {
        spidev = AH_SPI_HOST;
    }

    s_device = open(spidev, O_RDWR);
    if (s_device == -1)
    {
        printf("Failed to open %s\n", AH_SPI_HOST);
        return false;
    }

    char *spifreq = getenv("AH_SPI_FREQ");
    if (spifreq)
    {
        SPI_FREQUENCY = atoi(spifreq);
    }

    ioctl(s_device, SPI_IOC_WR_MODE, &SPI_MODE);
    ioctl(s_device, SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS_PER_WORD);
    ioctl(s_device, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_FREQUENCY);

    return true;
}

void ah_spi_deinit(void)
{
    if (s_device != -1)
    {
        close(s_device);
        s_device = -1;
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

    ioctl(s_device, SPI_IOC_MESSAGE(1), &tr);
}
