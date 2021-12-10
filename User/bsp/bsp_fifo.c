#include <stdbool.h>
#include "bsp_fifo.h"
#include <stdlib.h>
#include <string.h>

static uint32_t fifoWritePos = 0;
static uint32_t fifoReadPos = 0;
static uint8_t  fifoBuf[FIFO_MAX_SIZE];

bool bsp_FIFO_PushIn(uint8_t *data, uint32_t len)
{
    /* check data valid */
    if((!len) || (NULL == data) || (len > FIFO_MAX_SIZE))
    {
        return false;
    }

    /* check is fifo full*/
    uint32_t fifolen;
    bsp_FIFO_Len(&fifolen);
    if(len > FIFO_MAX_SIZE-fifolen)
    {
        return false;
    }

    /* push data into fifo */
    if(len > (FIFO_MAX_SIZE - fifoWritePos))
    {
        memcpy(&fifoBuf[fifoWritePos], data, FIFO_MAX_SIZE - fifoWritePos);
        memcpy(fifoBuf, &data[FIFO_MAX_SIZE - fifoWritePos], len - (FIFO_MAX_SIZE - fifoWritePos));
        fifoWritePos = len - (FIFO_MAX_SIZE - fifoWritePos);
    }
    else
    {
        memcpy(&fifoBuf[fifoWritePos], data, len);
        fifoWritePos = (fifoWritePos + len) % FIFO_MAX_SIZE;
    }
    return true;
} 

bool bsp_FIFO_PullOut(uint8_t * data, uint32_t len)
{
    /* check data valid */
    if((!len) || (NULL == data))
    {
        return false;
    }
    
    /* check is fifo enough to pull out */
    uint32_t fifolen;
    bsp_FIFO_Len(&fifolen);
    if(len > fifolen)
    {
        return false;
    }
    
    /* pull out data */
    if(len > (FIFO_MAX_SIZE - fifoReadPos))
    {
        memcpy(data, &fifoBuf[fifoReadPos], (FIFO_MAX_SIZE - fifoReadPos));
        memcpy(&data[FIFO_MAX_SIZE - fifoReadPos], fifoBuf, len - (FIFO_MAX_SIZE - fifoReadPos));
        fifoReadPos = len - (FIFO_MAX_SIZE - fifoReadPos);
    }
    else
    {
        memcpy(data, &fifoBuf[fifoReadPos],len);
        fifoReadPos = (fifoReadPos + len) % FIFO_MAX_SIZE;
    }
    return true;
}

void bsp_FIFO_Len(uint32_t *len)
{
    *len = (FIFO_MAX_SIZE - fifoReadPos + fifoWritePos) % FIFO_MAX_SIZE;
}