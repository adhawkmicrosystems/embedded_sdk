cmake_minimum_required(VERSION 3.22)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME stm32wb55)

include("vendor/gcc-arm-none-eabi.cmake")

enable_language(C ASM)

project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(vendor)

target_link_libraries(${CMAKE_PROJECT_NAME}
    vendor
)

get_filename_component(ROOT_DIR ../.. ABSOLUTE)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${ROOT_DIR}/bsp/inc/
    ${ROOT_DIR}/inc/
    ${ROOT_DIR}/examples/common/
)

FILE(GLOB sdk_sources ${ROOT_DIR}/src/*.c)
FILE(GLOB stm32_bsp_sources ${ROOT_DIR}/bsp/stm32/*.c  ${ROOT_DIR}/bsp/freertos/*.c)
FILE(GLOB app_sources src/*.c  ${ROOT_DIR}/examples/common/*.c)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${sdk_sources}
    ${stm32_bsp_sources}
    ${app_sources}
)

add_compile_definitions(STM32_UART_LOGGING=1)
