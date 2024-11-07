# Raspberry PI 4 Example Application

This example will proxy packets received over TCP to the tracker via SPI, and vise versa.

Make sure to enable SPI on the RPI 4. Open the Raspberry Pi Configuration -> Interfaces -> SPI.

## SPI Setup

Modify the `ah_spi_pins.h` header with the correct pin configuration for your device. Alternatively set the values using environment variables.

## Using the command line

To build:
`cmake -B build`
`cmake --build build`
To run: `./build/rpi_ip_proxy <TCP port>`
