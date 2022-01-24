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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define MX_PWR_MOTOR_Pin GPIO_PIN_4
#define MX_PWR_MOTOR_GPIO_Port GPIOE
#define MX_BUTTON_PWR_Pin GPIO_PIN_5
#define MX_BUTTON_PWR_GPIO_Port GPIOE
#define MX_LED_PWR_BUTTON_Pin GPIO_PIN_6
#define MX_LED_PWR_BUTTON_GPIO_Port GPIOE
#define MX_SPIFLASH_WP_Pin GPIO_PIN_14
#define MX_SPIFLASH_WP_GPIO_Port GPIOB
#define MX_PWR_TASK_Pin GPIO_PIN_11
#define MX_PWR_TASK_GPIO_Port GPIOD
#define MX_PWR_ROUTER_Pin GPIO_PIN_15
#define MX_PWR_ROUTER_GPIO_Port GPIOD
#define MX_PWR_RADAR_Pin GPIO_PIN_8
#define MX_PWR_RADAR_GPIO_Port GPIOA
#define MX_LED_SYS_Pin GPIO_PIN_15
#define MX_LED_SYS_GPIO_Port GPIOA
#define MX_USART3_TX_DEBUG_Pin GPIO_PIN_10
#define MX_USART3_TX_DEBUG_GPIO_Port GPIOC
#define MX_USART3_RX_DEBUG_Pin GPIO_PIN_11
#define MX_USART3_RX_DEBUG_GPIO_Port GPIOC
#define MX_PWR_MISC2_Pin GPIO_PIN_4
#define MX_PWR_MISC2_GPIO_Port GPIOB
#define MX_PWR_MISC1_Pin GPIO_PIN_8
#define MX_PWR_MISC1_GPIO_Port GPIOB
#define MX_SPIFLASH_CS_Pin GPIO_PIN_9
#define MX_SPIFLASH_CS_GPIO_Port GPIOB
#define MX_PWR_SOC_Pin GPIO_PIN_0
#define MX_PWR_SOC_GPIO_Port GPIOE
#define MX_PWR_USOUND_Pin GPIO_PIN_1
#define MX_PWR_USOUND_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
