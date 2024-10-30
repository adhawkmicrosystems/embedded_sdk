#include "gpio_rpi.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BLOCK_SIZE (4 * 1024)

// I/O access
volatile unsigned *s_gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(s_gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(s_gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3))
#define GPIO_SET *(s_gpio + 7)                   // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(s_gpio + 10)                  // clears bits which are 1 ignores bits which are 0
#define GET_GPIO(g) (*(s_gpio + 13) & (1 << g))  // 0 if LOW, (1<<g) if HIGH
#define GPIO_PULL *(s_gpio + 37)                 // Pull up/pull down
#define GPIO_PULLCLK0 *(s_gpio + 38)             // Pull up/pull down clock

bool ah_gpioInit(void)
{
    int mem_fd;
    void *gpio_map;

    if ((mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) < 0)
    {
        printf("Failed to open /dev/gpiomem\n");
        return false;
    }

    gpio_map = mmap(NULL,                    //Any adddress in our space will do
                    BLOCK_SIZE,              //Map length
                    PROT_READ | PROT_WRITE,  // Enable reading & writting to mapped memory
                    MAP_SHARED,              //Shared with other processes
                    mem_fd,                  //File to map
                    0                        //Offset to GPIO peripheral
    );

    close(mem_fd);  //No need to keep mem_fd open after mmap

    if (gpio_map == MAP_FAILED)
    {
        printf("mmap failed\n");
        return false;
    }

    // Always use volatile pointer!
    s_gpio = (volatile unsigned *)gpio_map;
    return true;
}

void ah_gpioSetDirection(unsigned pinIndex, enum GPIO_DIR direction)
{
    INP_GPIO(pinIndex);  // INP_GPIO should be set before the OUT_GPIO
    if (direction == GPIO_DIR_OUTPUT)
    {
        OUT_GPIO(pinIndex);
    }
}

void ah_gpioWritePin(unsigned pinIndex, enum GPIO_STATE state)
{
    if (state == GPIO_PIN_SET)
    {
        GPIO_SET = 1 << pinIndex;
    }
    else
    {
        GPIO_CLR = 1 << pinIndex;
    }
}
