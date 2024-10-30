# AdHawk Embedded SDK

The SDK library implements the functionality required to build an application which controls and collects data from an AdHawk Eye Tracker.

## Board Support

There are sample board support packages (bsp) for Zephyr / ESP32 / FreeRTOS / Linux. These can be modified as needed. If your board is different they can be used as reference implementations.

## Examples

To get up and running quickly refer to the `examples` folder. These implement a basic application as well as the required build infrastructure.

## Connecting to AdHawk's Wireless Mindlink hardware

AdHawk's Wireless Mindlink hardware is useful for quickly testing and bringing up an eye tracking application. It can be easily connected to a devboard or hardware of your choice.

To connect the AdHawk eye tracking hardware to your devboard:
- Connect the SPI breakout board to the AdHawk board using the provided ribbon cable
- Connect the GND, CLK, MISO, MOSI, CS (SS) pins of the SPI breakout board to your devboard
- Connect the USB / Battery *OR* connect the PWR pin to power the AdHawk eye tracking hardware

There are two firmwares for the AdHawk's Wireless Mindlink hardware:
- The first (kakapo_v4) implements our wireless / usb product and can be used to test and configure the eye tracker
- The second (kakapo_v4_sdk) is for SDK development. This firmware simply powers the eye tracker and disables everything else

Currently an nRF programmer is required to switch between the two different firmwares.
