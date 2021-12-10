/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PGA_MISO_Pin GPIO_PIN_2
#define PGA_MISO_GPIO_Port GPIOC
#define PGA_MOSI_Pin GPIO_PIN_3
#define PGA_MOSI_GPIO_Port GPIOC
#define RS485_DR_Pin GPIO_PIN_1
#define RS485_DR_GPIO_Port GPIOA
#define NAND_CS_Pin GPIO_PIN_4
#define NAND_CS_GPIO_Port GPIOC
#define ANA_PWR_Pin GPIO_PIN_5
#define ANA_PWR_GPIO_Port GPIOC
#define PGA_SHDN_Pin GPIO_PIN_0
#define PGA_SHDN_GPIO_Port GPIOB
#define PGA2_CS_Pin GPIO_PIN_1
#define PGA2_CS_GPIO_Port GPIOB
#define PGA1_CS_Pin GPIO_PIN_2
#define PGA1_CS_GPIO_Port GPIOB
#define PGA_SCK_Pin GPIO_PIN_10
#define PGA_SCK_GPIO_Port GPIOB
#define FIRE_Pin GPIO_PIN_13
#define FIRE_GPIO_Port GPIOB
#define ADC_PWR_Pin GPIO_PIN_9
#define ADC_PWR_GPIO_Port GPIOA
#define ADC_BUSY_Pin GPIO_PIN_10
#define ADC_BUSY_GPIO_Port GPIOA
#define ADC_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define ADC_FRSTDATA_Pin GPIO_PIN_11
#define ADC_FRSTDATA_GPIO_Port GPIOA
#define ADC_REF_Pin GPIO_PIN_12
#define ADC_REF_GPIO_Port GPIOA
#define ADC_CS_Pin GPIO_PIN_15
#define ADC_CS_GPIO_Port GPIOA
#define ADC_SCKB_Pin GPIO_PIN_10
#define ADC_SCKB_GPIO_Port GPIOC
#define ADC_RESET_Pin GPIO_PIN_12
#define ADC_RESET_GPIO_Port GPIOC
#define ADC_CONVB_Pin GPIO_PIN_3
#define ADC_CONVB_GPIO_Port GPIOB
#define ADC_CONVA_Pin GPIO_PIN_4
#define ADC_CONVA_GPIO_Port GPIOB
#define ADC_STBY_Pin GPIO_PIN_5
#define ADC_STBY_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
