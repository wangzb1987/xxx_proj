
#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "cmsis_os.h"

//#pragma anon_unions //KEIL编译器支持的形式


/* USER CODE BEGIN 0*/

#include "stm32l0xx.h"


/* USER CODE END   0*/

/* USER CODE BEGIN 1*/

typedef struct {
    GPIO_TypeDef* port;
    uint16_t      pin;
}GpioHandle;

extern unsigned int EnterCritical(void);
extern void ExitCritical(unsigned int dummy);
extern void GPIOInitHelper(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef *GPIO_Init);
extern void GPIODeInitHelper(GPIO_TypeDef* GPIOx, uint32_t pin);

#define BspErr()

/* USER CODE END 1*/



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


