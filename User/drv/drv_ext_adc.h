#ifndef DRV_EXT_ADC_H___
#define DRV_EXT_ADC_H___

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 


extern bool Uart1CmdToExtAdc(uint8_t* buf);
extern bool Uart1CmdPlug1Read(void);
extern bool Uart1CmdPlug2Read(void);

extern void AD7124PwrOff(void);
extern void ADC_ReadID(void);


#ifdef __cplusplus
}
#endif

#endif