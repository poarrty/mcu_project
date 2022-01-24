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
#define CL_ADD_WATER_VALVE_CTRL_EN_Pin GPIO_PIN_13
#define CL_ADD_WATER_VALVE_CTRL_EN_GPIO_Port GPIOC
#define CL_TRANSITION_BOX_VALVE_CTRLL_EN_Pin GPIO_PIN_14
#define CL_TRANSITION_BOX_VALVE_CTRLL_EN_GPIO_Port GPIOC
#define CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_Pin GPIO_PIN_15
#define CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_GPIO_Port GPIOC
#define AUTO_CHARGE_OUTPUT_EN_Pin GPIO_PIN_0
#define AUTO_CHARGE_OUTPUT_EN_GPIO_Port GPIOC
#define STOP_CHARGE_CTRL_KEY_Pin GPIO_PIN_1
#define STOP_CHARGE_CTRL_KEY_GPIO_Port GPIOC
#define CHARGE_ELECTRODE_STATUS_KEY_Pin GPIO_PIN_2
#define CHARGE_ELECTRODE_STATUS_KEY_GPIO_Port GPIOC
#define CHARGER_FORCE_OUTPUT_KEY_Pin GPIO_PIN_3
#define CHARGER_FORCE_OUTPUT_KEY_GPIO_Port GPIOC
#define TIM2_PWM_OUTPUT_CHANNEL_1_Pin GPIO_PIN_0
#define TIM2_PWM_OUTPUT_CHANNEL_1_GPIO_Port GPIOA
#define TIM2_PWM_OUTPUT_CHANNEL_2_Pin GPIO_PIN_1
#define TIM2_PWM_OUTPUT_CHANNEL_2_GPIO_Port GPIOA
#define CHARGER_OUTPUT_EN_Pin GPIO_PIN_4
#define CHARGER_OUTPUT_EN_GPIO_Port GPIOA
#define DIGITAL_TUBE_LOCK_EN_Pin GPIO_PIN_6
#define DIGITAL_TUBE_LOCK_EN_GPIO_Port GPIOA
#define TEMPERTURE_DETECT_Pin GPIO_PIN_4
#define TEMPERTURE_DETECT_GPIO_Port GPIOC
#define LED_G_EN_Pin GPIO_PIN_5
#define LED_G_EN_GPIO_Port GPIOC
#define USART3_RS485_DE_Pin GPIO_PIN_0
#define USART3_RS485_DE_GPIO_Port GPIOB
#define E2PROM_WP_EN_Pin GPIO_PIN_1
#define E2PROM_WP_EN_GPIO_Port GPIOB
#define CL_CLEAR_WATER_PUMP_CTRL_EN_Pin GPIO_PIN_12
#define CL_CLEAR_WATER_PUMP_CTRL_EN_GPIO_Port GPIOB
#define CL_SEWAGE_PUMP_CTRL_EN_Pin GPIO_PIN_13
#define CL_SEWAGE_PUMP_CTRL_EN_GPIO_Port GPIOB
#define WIRE_CHARGE_OUTPUT_EN_Pin GPIO_PIN_15
#define WIRE_CHARGE_OUTPUT_EN_GPIO_Port GPIOB
#define WIFI_PWR_EN_Pin GPIO_PIN_9
#define WIFI_PWR_EN_GPIO_Port GPIOC
#define DIGITAL_TUBE_PWR_EN_Pin GPIO_PIN_15
#define DIGITAL_TUBE_PWR_EN_GPIO_Port GPIOA
#define IRDA_TX_Pin GPIO_PIN_10
#define IRDA_TX_GPIO_Port GPIOC
#define IRDA_RX_Pin GPIO_PIN_11
#define IRDA_RX_GPIO_Port GPIOC
#define BT_POWER_EN_Pin GPIO_PIN_3
#define BT_POWER_EN_GPIO_Port GPIOB
#define CL_WATER_LEVEL_SENSOR_STATUS_DETECT_Pin GPIO_PIN_4
#define CL_WATER_LEVEL_SENSOR_STATUS_DETECT_GPIO_Port GPIOB
#define LED_R_EN_Pin GPIO_PIN_5
#define LED_R_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
