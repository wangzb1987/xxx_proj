#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "bsp_tim2.h"

extern osSemaphoreId g_TIM2_AlarmHandle;

void xHandleTIM2AlarmProc(void const * argument)
{
    for(;;)
    {
        xSemaphoreTake(g_TIM2_AlarmHandle, portMAX_DELAY);
        /* add period process code here */

    }
}
