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
#define MX_IO_SENSOR_EMG_Pin               GPIO_PIN_2
#define MX_IO_SENSOR_EMG_GPIO_Port         GPIOE
#define MX_IO_SENSOR_CRASH_Pin             GPIO_PIN_3
#define MX_IO_SENSOR_CRASH_GPIO_Port       GPIOE
#define MX_PWR_MOTOR_Pin                   GPIO_PIN_4
#define MX_PWR_MOTOR_GPIO_Port             GPIOE
#define MX_BUTTON_PWR_Pin                  GPIO_PIN_5
#define MX_BUTTON_PWR_GPIO_Port            GPIOE
#define MX_LED_PWR_BUTTON_Pin              GPIO_PIN_6
#define MX_LED_PWR_BUTTON_GPIO_Port        GPIOE
#define MX_IO_SENSOR_RAIN1_Pin             GPIO_PIN_0
#define MX_IO_SENSOR_RAIN1_GPIO_Port       GPIOC
#define MX_CHARGE_RELAY_CTRL_Pin           GPIO_PIN_1
#define MX_CHARGE_RELAY_CTRL_GPIO_Port     GPIOC
#define MX_IO_SENSOR_RAIN2_Pin             GPIO_PIN_3
#define MX_IO_SENSOR_RAIN2_GPIO_Port       GPIOC
#define MX_UART4_TX_USOUND_NVG_Pin         GPIO_PIN_0
#define MX_UART4_TX_USOUND_NVG_GPIO_Port   GPIOA
#define MX_UART4_RX_USOUND_NVG_Pin         GPIO_PIN_1
#define MX_UART4_RX_USOUND_NVG_GPIO_Port   GPIOA
#define MX_IR_SIM_UART_RX1_Pin             GPIO_PIN_2
#define MX_IR_SIM_UART_RX1_GPIO_Port       GPIOA
#define MX_IR_SIM_UART_RX1_EXTI_IRQn       EXTI2_IRQn
#define MX_IR_SIM_UART_RX2_Pin             GPIO_PIN_3
#define MX_IR_SIM_UART_RX2_GPIO_Port       GPIOA
#define MX_IR_SIM_UART_RX2_EXTI_IRQn       EXTI3_IRQn
#define MX_IO_SENSOR_FLOOD_Pin             GPIO_PIN_4
#define MX_IO_SENSOR_FLOOD_GPIO_Port       GPIOA
#define MX_SPI1_SCK_DISPLAY_Pin            GPIO_PIN_5
#define MX_SPI1_SCK_DISPLAY_GPIO_Port      GPIOA
#define MX_SPI1_MOSI_DISPLAY_Pin           GPIO_PIN_7
#define MX_SPI1_MOSI_DISPLAY_GPIO_Port     GPIOA
#define MX_ADC1_TEMP_Pin                   GPIO_PIN_1
#define MX_ADC1_TEMP_GPIO_Port             GPIOB
#define MX_LED_LEFT_B_Pin                  GPIO_PIN_2
#define MX_LED_LEFT_B_GPIO_Port            GPIOB
#define MX_DISPLAY_LOCK_Pin                GPIO_PIN_7
#define MX_DISPLAY_LOCK_GPIO_Port          GPIOE
#define MX_TIM1_CH1_IR_TX1_Pin             GPIO_PIN_9
#define MX_TIM1_CH1_IR_TX1_GPIO_Port       GPIOE
#define MX_LED_LEFT_G_Pin                  GPIO_PIN_10
#define MX_LED_LEFT_G_GPIO_Port            GPIOE
#define MX_LED_LEFT_R_Pin                  GPIO_PIN_11
#define MX_LED_LEFT_R_GPIO_Port            GPIOE
#define MX_LED_RIGHT_G_Pin                 GPIO_PIN_12
#define MX_LED_RIGHT_G_GPIO_Port           GPIOE
#define MX_LED_RIGHT_R_Pin                 GPIO_PIN_13
#define MX_LED_RIGHT_R_GPIO_Port           GPIOE
#define MX_CHARGE_DET_MANUAL_Pin           GPIO_PIN_14
#define MX_CHARGE_DET_MANUAL_GPIO_Port     GPIOE
#define MX_CHARGE_DET_Pin                  GPIO_PIN_15
#define MX_CHARGE_DET_GPIO_Port            GPIOE
#define MX_LED_EMERG_Pin                   GPIO_PIN_11
#define MX_LED_EMERG_GPIO_Port             GPIOB
#define MX_CAN2_RX_BATTERY_Pin             GPIO_PIN_12
#define MX_CAN2_RX_BATTERY_GPIO_Port       GPIOB
#define MX_CAN2_TX_BATTERY_Pin             GPIO_PIN_13
#define MX_CAN2_TX_BATTERY_GPIO_Port       GPIOB
#define MX_SPIFLASH_WP_Pin                 GPIO_PIN_14
#define MX_SPIFLASH_WP_GPIO_Port           GPIOB
#define MX_LUNA_UART_SOFT_RX_Pin           GPIO_PIN_9
#define MX_LUNA_UART_SOFT_RX_GPIO_Port     GPIOD
#define MX_LUNA_UART_SOFT_RX_EXTI_IRQn     EXTI9_5_IRQn
#define MX_PWR_TASK_Pin                    GPIO_PIN_11
#define MX_PWR_TASK_GPIO_Port              GPIOD
#define MX_LED_RIGHT_B_Pin                 GPIO_PIN_12
#define MX_LED_RIGHT_B_GPIO_Port           GPIOD
#define MX_LED_DRIVE_Pin                   GPIO_PIN_13
#define MX_LED_DRIVE_GPIO_Port             GPIOD
#define MX_NVG_UART_SW2_Pin                GPIO_PIN_14
#define MX_NVG_UART_SW2_GPIO_Port          GPIOD
#define MX_PWR_ROUTER_Pin                  GPIO_PIN_15
#define MX_PWR_ROUTER_GPIO_Port            GPIOD
#define MX_NVG_UART_SW1_Pin                GPIO_PIN_8
#define MX_NVG_UART_SW1_GPIO_Port          GPIOC
#define MX_LED_PWR_BUTTON_GREEN_Pin        GPIO_PIN_9
#define MX_LED_PWR_BUTTON_GREEN_GPIO_Port  GPIOC
#define MX_PWR_RADAR_Pin                   GPIO_PIN_8
#define MX_PWR_RADAR_GPIO_Port             GPIOA
#define MX_USART1_TX_SOC_Pin               GPIO_PIN_9
#define MX_USART1_TX_SOC_GPIO_Port         GPIOA
#define MX_USART1_RX_SOC_Pin               GPIO_PIN_10
#define MX_USART1_RX_SOC_GPIO_Port         GPIOA
#define MX_CAN1_RX_MOTOR_Pin               GPIO_PIN_11
#define MX_CAN1_RX_MOTOR_GPIO_Port         GPIOA
#define MX_CAN1_TX_MOTOR_Pin               GPIO_PIN_12
#define MX_CAN1_TX_MOTOR_GPIO_Port         GPIOA
#define MX_LED_RUN_Pin                     GPIO_PIN_15
#define MX_LED_RUN_GPIO_Port               GPIOA
#define MX_USART3_TX_DEBUG_Pin             GPIO_PIN_10
#define MX_USART3_TX_DEBUG_GPIO_Port       GPIOC
#define MX_USART3_RX_DEBUG_Pin             GPIO_PIN_11
#define MX_USART3_RX_DEBUG_GPIO_Port       GPIOC
#define MX_UART5_TX_IMU_Pin                GPIO_PIN_12
#define MX_UART5_TX_IMU_GPIO_Port          GPIOC
#define MX_UART5_RX_IMU_Pin                GPIO_PIN_2
#define MX_UART5_RX_IMU_GPIO_Port          GPIOD
#define MX_IMU_RSTN_Pin                    GPIO_PIN_3
#define MX_IMU_RSTN_GPIO_Port              GPIOD
#define MX_IMU_TX_ENABLE_Pin               GPIO_PIN_4
#define MX_IMU_TX_ENABLE_GPIO_Port         GPIOD
#define MX_USART2_TX_USOUND_TASK_Pin       GPIO_PIN_5
#define MX_USART2_TX_USOUND_TASK_GPIO_Port GPIOD
#define MX_USART2_RX_USOUND_TASK_Pin       GPIO_PIN_6
#define MX_USART2_RX_USOUND_TASK_GPIO_Port GPIOD
#define MX_PWR_HEAT_Pin                    GPIO_PIN_7
#define MX_PWR_HEAT_GPIO_Port              GPIOD
#define MX_PWR_MISC2_Pin                   GPIO_PIN_4
#define MX_PWR_MISC2_GPIO_Port             GPIOB
#define MX_I2C1_SCL_EEPROM_Pin             GPIO_PIN_6
#define MX_I2C1_SCL_EEPROM_GPIO_Port       GPIOB
#define MX_I2C1_SDA_EEPROM_Pin             GPIO_PIN_7
#define MX_I2C1_SDA_EEPROM_GPIO_Port       GPIOB
#define MX_PWR_MISC1_Pin                   GPIO_PIN_8
#define MX_PWR_MISC1_GPIO_Port             GPIOB
#define MX_SPIFLASH_CS_Pin                 GPIO_PIN_9
#define MX_SPIFLASH_CS_GPIO_Port           GPIOB
#define MX_PWR_SOC_Pin                     GPIO_PIN_0
#define MX_PWR_SOC_GPIO_Port               GPIOE
#define MX_PWR_USOUND_Pin                  GPIO_PIN_1
#define MX_PWR_USOUND_GPIO_Port            GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
