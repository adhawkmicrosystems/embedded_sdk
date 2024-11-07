cmake_minimum_required(VERSION 3.22)

set(CMAKE_PROJECT_NAME adhawk_example)

project(${CMAKE_PROJECT_NAME})

add_executable(${CMAKE_PROJECT_NAME})

get_filename_component(ROOT_DIR ../.. ABSOLUTE)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    src/
    ${ROOT_DIR}/bsp/inc/
    ${ROOT_DIR}/inc/
    ${ROOT_DIR}/examples/common/
)

FILE(GLOB sdk_sources ${ROOT_DIR}/src/*.c)
FILE(GLOB rpi_bsp_sources ${ROOT_DIR}/bsp/linux/*.c )
FILE(GLOB app_sources src/*.c  ${ROOT_DIR}/examples/common/*.c)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${sdk_sources}
    ${rpi_bsp_sources}
    ${app_sources}
)
