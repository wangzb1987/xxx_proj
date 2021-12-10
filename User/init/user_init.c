#include "gpio.h"
#include "i2c.h"
#include "drv_ext_eeprom.h"
#include "drv_ext_adc.h"
#include "drv_ble.h"
#include "drv_nfc.h"

#include "main.h"

void user_init(void)
{
    /* DeInit EEPROM */
    EEPROM_DeInit();
    /* DeInit ADC*/
    AD7124PwrOff();

    //BLE_DeInit();
    // BLE_DeInit();
    
    // NFC_DeInit();
    MX_I2C1_DeInit();

    /* DeInit I2C */

    /* DeInit GPIOs*/
}