#pragma once

#include <stdbool.h>

enum GPIO_DIR
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT,
};

enum GPIO_STATE
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET,
};

bool ah_gpioInit(void);
void ah_gpioSetDirection(unsigned pinIndex, enum GPIO_DIR direction);
void ah_gpioWritePin(unsigned pinIndex, enum GPIO_STATE state);
