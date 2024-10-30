#include "bsp_spi.h"

#include "freertos/FreeRTOS.h"
#include "stm32wbxx_hal.h"

#define SPI_THREAD_STACK_SIZE 4096
#define SPI_THREAD_PRIORITY (configMAX_PRIORITIES - 2)

#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_4

static SPI_HandleTypeDef s_hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;

static bool MX_SPI1_Init(void)
{
    /* SPI1 parameter configuration*/
    s_hspi1.Instance = SPI1;
    s_hspi1.Init.Mode = SPI_MODE_MASTER;
    s_hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    s_hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    s_hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    s_hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    s_hspi1.Init.NSS = SPI_NSS_SOFT;
    s_hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    s_hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    s_hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    s_hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    s_hspi1.Init.CRCPolynomial = 7;
    s_hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    s_hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&s_hspi1) != HAL_OK)
    {
        return false;
    }

    // initialize the CS pin
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, CS_PIN, GPIO_PIN_SET);

    /*Configure GPIO pin : PA4 */
    GPIO_InitStruct.Pin = CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS_PORT, &GPIO_InitStruct);

    return true;
}

static void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    /* DMA1_Channel2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

bool ah_spi_init(void)
{
    MX_DMA_Init();

    if (!MX_SPI1_Init())
    {
        return false;
    }

    return true;
}

void ah_spi_deinit(void)
{
    HAL_SPI_MspDeInit(&s_hspi1);
    HAL_SPI_DeInit(&s_hspi1);
}

int ah_spi_getThreadPriority(void)
{
    return SPI_THREAD_PRIORITY;
}

void ah_spi_transferFullDuplex(uint8_t txFrame[AH_SPI_FRAME_SIZE], uint8_t rxFrame[AH_SPI_FRAME_SIZE])
{
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&s_hspi1, txFrame, rxFrame, AH_SPI_FRAME_SIZE, 1000);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}
