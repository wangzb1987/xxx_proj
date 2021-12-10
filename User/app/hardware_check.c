/***************************************************************************//**
*   @file    hardware_check.c
*   @brief   check hardware status
*
********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "drv_ext_eeprom.h"
#include "hardware_check.h"
#include "drv_ext_adc.h"
#include "drv_ble.h"
#include "drv_nfc.h"
#include "rtc.h"

void xHandleBootCheck(void const * argument)
//void systemBootCheck(void)
{
    for(;;)
    {
        osDelay(10);
        #if SYSTEM_BOOT_CHECK
            if(!M95M02_CheckID())
            {
                DbgTrace("EEPROM ID CHECK FAILLED!\r\n");
            }
            else
            {
                DbgTrace("EEPROM ID CHECK PASSED!\r\n");
            }
            /* add other peripherals check code here  */
            //DbgTrace("NFC check id start time is : %d\r\n", osKernelSysTick());
            NFC_ReadID();
            //DbgTrace("NFC check id stop time is : %d\r\n", osKernelSysTick());
            ADC_ReadID();
            //DbgTrace("ADC check id stop time is : %d\r\n", osKernelSysTick());
            //BLE_ReadID();
            osDelay(25);
            /*-------------*/
        #endif
        /* add other check code here  */
        #if MEMORY_CHECK
            DbgTrace("Memory check started:\r\n");
            uint32_t addr = 0;
            uint16_t pageChecked = 0;
            uint32_t err = 0;
            uint8_t txBuf[256];
            uint8_t rxBuf[256];

            for( ; pageChecked < 1024; pageChecked++)
            {
                /* Initialize the page write data*/
                for(uint16_t i=0; i<256; i++)
                {
                    txBuf[i] = (uint8_t)(i + pageChecked);
                }
                /* write data into EEPROM and check the write status*/
                if(!M95M02_WriteData(addr, txBuf, 256))    
                {
                    DbgTrace("Page %d write failed!\r\n", pageChecked);
                }

                /* read data from EEPROM and check the read status*/
                if(!M95M02_ReadData(addr, rxBuf, 256))
                {
                    DbgTrace("Page %d read failed!\r\n", pageChecked);     
                }

                /* compare write data and read data*/
                for(uint16_t i=0; i<256; i++)
                {
                    if(txBuf[i] != rxBuf[i])
                    {
                        err ++;
                    }
                }

                /* move to the next page*/
                addr = pageChecked << 8;

                if((pageChecked % 10) == 0)
                {
                    DbgTrace("All of %d%% of memeory has checked with %d error!\r\n", (uint16_t)(pageChecked/10.24f), err);
                }
            }
            //checkPercent = (float)pageChecked/10.24f;
            DbgTrace("All of 100%% of memeory has checked with %d error!\r\n", err);

        #endif   
        user_RTC_WakeupTimerStart_IT();
        vTaskDelete(NULL);
    }
}