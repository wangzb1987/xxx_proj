#ifndef BSP_FIFO_H__
#define BSP_FIFO_H__

#include "main.h"
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define FIFO_MAX_SIZE   (128)

bool bsp_FIFO_PushIn(uint8_t *data, uint32_t len);
bool bsp_FIFO_PullOut(uint8_t * data, uint32_t len);
void bsp_FIFO_Len(uint32_t *len);


#ifdef __cplusplus
}
#endif

#endif