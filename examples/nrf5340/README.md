# nRF5340 + Zephyr Example Application

Install and setup the nRF Connect SDK v2.6.99 by following the instructions here: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation/assistant.html

This example was tested against the `nrf5340dk_nrf5340_cpuapp` board.

## Using VSCode

Install the VSCode plugins: https://nrfconnect.github.io/vscode-nrf-connect/

## Using the command line

To build / flash from the command line run: `west build -b nrf5340dk_nrf5340_cpuapp && west flash`

## SPI Setup

This example uses the default nrf5340 spi4 pins:
- SCK: P1.15
- MISO: P1.14
- MOSI: P1.13
- CS: P1.12

These can be modified by adding an overlay file with with correct pins for your device.
