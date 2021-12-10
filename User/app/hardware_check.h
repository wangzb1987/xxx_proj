/***************************************************************************//**
*   @file    hardware_check.h
*   @brief   check hardware status
*
********************************************************************************/

#ifndef __HARDWARE_CHECK_H
#define __HARDWARE_CHECK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" 
{
#endif

#define SYSTEM_BOOT_CHECK   (1)
#define MEMORY_CHECK        (0)

// extern void systemBootCheck(void);

#ifdef __cplusplus
}
#endif

#endif  /* __HARDWARE_CHECK_H */