# Raspberry PI 4 Example Application

Make sure to enable SPI on the RPI 4. Open the Raspberry Pi Configuration -> Interfaces -> SPI.

## SPI Setup

Modify the `ah_spi_pins.h` header with the correct pin configuration for your device. Alternatively set the values using environment variables.

## Using the command line

To build:
`cmake -B build`
`cmake --build build`
To run: `./build/adhawk_example`
