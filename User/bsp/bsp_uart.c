#include "main.h"
#include <stdint.h>
#include "usart.h"
#include "bsp_uart.h"
#include "user_fifo.h"

#define UART_MAX_BUF_LEN        (256)
#define bsp_uart                huart2
#define BSP_UART_BASE           HUART2

// static uint8_t bsp_bt_uart_TxBuf[LPUART_MAX_BUF_LEN];
static uint8_t bsp_UART_RxBuf[UART_MAX_BUF_LEN];

extern UART_HandleTypeDef huart2;

extern osSemaphoreId ug_UART_RxCntHandle;

HAL_StatusTypeDef bsp_UART_SendData(uint32_t len, uint8_t *data)
{
    if((!len) || (NULL==data))
    {
        return HAL_ERROR;
    }
    #if(BSP_UART_DMA)
    {
        return HAL_UART_Transmit_DMA(&bsp_uart, data, len);
    }
    #elif(BSP_UART_IT)
    {
        return HAL_UART_Transmit_IT(&bsp_uart, data, len);
    }
    #else
    {
        return HAL_UART_Transmit(&bsp_uart, data, len, len+len);
    }
    #endif
}

HAL_StatusTypeDef bsp_UART_ReceiveData(void)
{
    #if(BSP_UART_DMA)
    {
        return HAL_UARTEx_ReceiveToIdle_DMA(&bsp_uart, bsp_UART_RxBuf, UART_MAX_BUF_LEN);
    }
    #elif(BSP_UART_IT)
    {
        return HAL_UARTEx_ReceiveToIdle_IT(&bsp_uart, bsp_UART_RxBuf, UART_MAX_BUF_LEN);
    }
    #else
    {
        DbgTrace("DO NOT USE TIMEMODE TO RECEIVE DATA!\r\n");
        return HAL_ERROR;
    }
    #endif
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(BSP_UART_BASE == huart->Instance)
    {
        bsp_FIFO_PushIn(bsp_UART_RxBuf, Size);
        DbgTrace("bt received %d datas!\r\n", Size);
        /* clear read register*/
        bsp_UART_ReceiveData();
    }
}

