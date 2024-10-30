get_filename_component(ROOT_DIR ../../.. ABSOLUTE)

FILE(GLOB app_sources *.c ${ROOT_DIR}/examples/common/*.c)
FILE(GLOB sdk_sources ${ROOT_DIR}/src/*.c)
FILE(GLOB esp_bsp_sources ${ROOT_DIR}/bsp/esp32/*.c ${ROOT_DIR}/bsp/freertos/*.c)

set(SOURCES
    ${app_sources}
    ${sdk_sources}
    ${esp_bsp_sources}
)

set(INCLUDES
    "${ROOT_DIR}/bsp/inc"
    "${ROOT_DIR}/inc"
    "${ROOT_DIR}/examples/common"
    "."
)

idf_component_register(
    SRCS
        ${SOURCES}
    INCLUDE_DIRS
        ${INCLUDES}
    REQUIRES
        driver
        esp_timer
)
