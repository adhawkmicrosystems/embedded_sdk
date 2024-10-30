# STM32 WB55 + FreeRTOS Example Application

This example uses the `NUCLEO-WB55RG` development board provided by STMicroelectronics.

## Environment Setup
- CMake
- OpenOCD

### Building
Navigate to the stm32wb55 example folder

`cd examples/stm32wb55`

Create build directory

```
mkdir build
cd build
cmake ../
```

To build, run
`cmake --build .`
in `/build`

### Flashing
Flashing can be done through OpenOCD.

To flash, connect to CN15 (ST-Link)on the WB55 board and run
```
openocd -f interface/stlink-v2-1.cfg -f target/stm32wbx.cfg -c "program build/stm32wb55.elf verify reset exit"
```
where `build/stm32wb55.elf` is replaced with the relative path to the .elf


## Hardware Setup

### SPI
The example uses the SPI_1 interface with the following pin mapping:

- SCK: PA5
- MISO: PA6
- MOSI: PA7
- CS: PA4

### Logging
Messages are sent through UART and can be read through a serial terminal at baud rate 115200. Ensure that `HAL_UART_MODULE_ENABLED` is true. Connect your PC to the board to the Micro-Usb connector CN15 (ST-Link) and use your terminal of choice (PuTTY, screen, etc).
