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
#include "stm32f1xx_hal.h"

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
#define HEAT_CTL_Pin GPIO_PIN_0
#define HEAT_CTL_GPIO_Port GPIOC
#define MCU_LED_Pin GPIO_PIN_1
#define MCU_LED_GPIO_Port GPIOC
#define DEBUG_USART_SW1_Pin GPIO_PIN_2
#define DEBUG_USART_SW1_GPIO_Port GPIOC
#define DEBUG_USART_SW2_Pin GPIO_PIN_3
#define DEBUG_USART_SW2_GPIO_Port GPIOC
#define UITRASONIC_ECHO_DETECT_Pin GPIO_PIN_0
#define UITRASONIC_ECHO_DETECT_GPIO_Port GPIOA
#define UITRASONIC_40KHZ_PULSE_OUTPUT_Pin GPIO_PIN_1
#define UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port GPIOA
#define UITRASONIC_CHANNEL_SEL_0_Pin GPIO_PIN_4
#define UITRASONIC_CHANNEL_SEL_0_GPIO_Port GPIOA
#define UITRASONIC_CHANNEL_SEL_1_Pin GPIO_PIN_5
#define UITRASONIC_CHANNEL_SEL_1_GPIO_Port GPIOA
#define UITRASONIC_CHANNEL_SEL_2_Pin GPIO_PIN_6
#define UITRASONIC_CHANNEL_SEL_2_GPIO_Port GPIOA
#define POWER_KEY_Pin GPIO_PIN_4
#define POWER_KEY_GPIO_Port GPIOC
#define POWER_KEY_EXTI_IRQn EXTI4_IRQn
#define RS485_USART3_DE_Pin GPIO_PIN_0
#define RS485_USART3_DE_GPIO_Port GPIOB
#define E2PROM_WP_EN_Pin GPIO_PIN_1
#define E2PROM_WP_EN_GPIO_Port GPIOB
#define XG_CAM_PWR_EN_Pin GPIO_PIN_7
#define XG_CAM_PWR_EN_GPIO_Port GPIOC
#define YT_CAM_PWR_EN_Pin GPIO_PIN_8
#define YT_CAM_PWR_EN_GPIO_Port GPIOC
#define UITRASONIC_PWR_EN_Pin GPIO_PIN_9
#define UITRASONIC_PWR_EN_GPIO_Port GPIOC
#define SYS_PWR_EN_Pin GPIO_PIN_8
#define SYS_PWR_EN_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
