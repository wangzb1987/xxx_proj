#ifndef BSP_UART_H___
#define BSP_UART_H___

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 

#define BSP_UART_DMA    (0)
#define BSP_UART_IT     (1)

HAL_StatusTypeDef bsp_UART_SendData(uint32_t len, uint8_t *data);


#ifdef __cplusplus
}
#endif

#endif