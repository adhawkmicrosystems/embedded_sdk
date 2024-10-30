cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(adhawk_example)

get_filename_component(ROOT_DIR ../.. ABSOLUTE)

target_include_directories(app PRIVATE
    src/
    ${ROOT_DIR}/bsp/inc/
    ${ROOT_DIR}/inc/
    ${ROOT_DIR}/examples/common/
)

FILE(GLOB sdk_sources ${ROOT_DIR}/src/*.c)
FILE(GLOB zephyr_bsp_sources ${ROOT_DIR}/bsp/zephyr/*.c)
FILE(GLOB app_sources src/*.c ${ROOT_DIR}/examples/common/*.c)

target_sources(app PRIVATE
    ${sdk_sources}
    ${zephyr_bsp_sources}
    ${app_sources}
)
