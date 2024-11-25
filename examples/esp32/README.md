# esp32 + freeRTOS Example Application

Install and setup the ESP32 SDK: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html#get-started-linux-macos-first-steps

This example was tested against the `esp32s3` board.

## Using the command line

To build: `idf.py build`

To flash: `idf.py flash monitor`

## SPI Setup

Mondify the `ah_spi_pins.h` header with the correct pin configuration for your device.
