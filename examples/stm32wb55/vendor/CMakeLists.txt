cmake_minimum_required(VERSION 3.22)

project(vendor)
add_library(vendor INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(vendor INTERFACE
	USE_HAL_DRIVER
	STM32WB55xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(vendor INTERFACE
    Core/Inc
    Drivers/STM32WBxx_HAL_Driver/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/include/freertos
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    Drivers/CMSIS/Device/ST/STM32WBxx/Include
    Drivers/CMSIS/Include
)

target_sources(vendor INTERFACE
    Core/Src/stm32wbxx_it.c
    Core/Src/stm32wbxx_hal_msp.c
    Core/Src/stm32wbxx_hal_timebase_tim.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_gpio.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_hsem.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cortex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_exti.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd_ex.c
    Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_usb.c
    Core/Src/system_stm32wbxx.c
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    startup_stm32wb55xx_cm4.s
)

target_link_directories(vendor INTERFACE
)

target_link_libraries(vendor INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


