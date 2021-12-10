#include "main.h"
#include "spi.h"
#include "gpio.h"




void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPeriorityTaskWoken = pdFALSE;
    if(SPI1 == hspi->Instance)
    {
        #if EEPROM_USE_DMA || EEPROM_USE_IT
            xSemaphoreGiveFromISR(g_EEPROM_TxDoneHandle, &xHigherPeriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPeriorityTaskWoken);
        #endif
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPeriorityTaskWoken = pdFALSE;
    if(SPI1 == hspi->Instance)
    {
        #if EEPROM_USE_DMA || EEPROM_USE_IT
            xSemaphoreGiveFromISR(g_EEPROM_TxRxDoneHandle,  &xHigherPeriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPeriorityTaskWoken);
        #endif
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPeriorityTaskWoken = pdFALSE;
    if(SPI1 == hspi->Instance)
    {
        #if EEPROM_USE_DMA || EEPROM_USE_IT
            xSemaphoreGiveFromISR(g_EEPROM_RxDoneHandle,  &xHigherPeriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPeriorityTaskWoken);
        #endif
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t xHigherPeriorityTaskWoken = pdFALSE;
    if(GPIO_Pin == ADC_MISO_Pin)
    {
        xSemaphoreGiveFromISR(g_EXTADC_DoneHandle, &xHigherPeriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPeriorityTaskWoken);
    }
}