#include "FreeRTOS.h"
#include "gpio.h"

static int inHandlerMode (void) {
    return __get_IPSR() != 0;
}

unsigned int EnterCritical(void) {
    unsigned int dummy = 0;
    if (inHandlerMode()) {
        dummy = portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else {
        vPortEnterCritical();
    }
    return dummy;
}

void ExitCritical(unsigned int dummy) {
    if (inHandlerMode()) {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(dummy);
    }
    else {
        vPortExitCritical();
    }
}

void GPIOInitHelper(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    unsigned int tmp = EnterCritical();
    HAL_GPIO_Init( GPIOx, GPIO_Init );
    ExitCritical(tmp);
}

void GPIODeInitHelper(GPIO_TypeDef* GPIOx, uint32_t pin) {
    unsigned int tmp = EnterCritical();
    HAL_GPIO_DeInit( GPIOx, pin );
    ExitCritical(tmp);
}


