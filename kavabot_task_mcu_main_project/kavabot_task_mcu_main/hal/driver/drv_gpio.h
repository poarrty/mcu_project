/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_gpio.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 15:06:44
 * @Description: GPIO底层接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "main.h"

#define UITRASONIC_40KHZ_PULSE_OUTPUT_H()                      \
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port, \
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_SET)
#define UITRASONIC_40KHZ_PULSE_OUTPUT_L()                      \
    HAL_GPIO_WritePin(UITRASONIC_40KHZ_PULSE_OUTPUT_GPIO_Port, \
                      UITRASONIC_40KHZ_PULSE_OUTPUT_Pin, GPIO_PIN_RESET)
#define UITRASONIC_CHANNEL_SEL_0_H()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_0_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_0_Pin, GPIO_PIN_SET)
#define UITRASONIC_CHANNEL_SEL_0_L()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_0_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_0_Pin, GPIO_PIN_RESET)
#define UITRASONIC_CHANNEL_SEL_1_H()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_1_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_1_Pin, GPIO_PIN_SET)
#define UITRASONIC_CHANNEL_SEL_1_L()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_1_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_1_Pin, GPIO_PIN_RESET)
#define UITRASONIC_CHANNEL_SEL_2_H()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_2_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_2_Pin, GPIO_PIN_SET)
#define UITRASONIC_CHANNEL_SEL_2_L()                      \
    HAL_GPIO_WritePin(UITRASONIC_CHANNEL_SEL_2_GPIO_Port, \
                      UITRASONIC_CHANNEL_SEL_2_Pin, GPIO_PIN_RESET)
#define RS485_USART3_DE_H()                                           \
    HAL_GPIO_WritePin(RS485_USART3_DE_GPIO_Port, RS485_USART3_DE_Pin, \
                      GPIO_PIN_SET)
#define RS485_USART3_DE_L()                                           \
    HAL_GPIO_WritePin(RS485_USART3_DE_GPIO_Port, RS485_USART3_DE_Pin, \
                      GPIO_PIN_RESET)
#define XG_CAM_PWR_ON() \
    HAL_GPIO_WritePin(XG_CAM_PWR_EN_GPIO_Port, XG_CAM_PWR_EN_Pin, GPIO_PIN_SET)
#define XG_CAM_PWR_OFF()                                          \
    HAL_GPIO_WritePin(XG_CAM_PWR_EN_GPIO_Port, XG_CAM_PWR_EN_Pin, \
                      GPIO_PIN_RESET)
#define XC_CAM_PWR_READ() \
    HAL_GPIO_ReadPin(XG_CAM_PWR_EN_GPIO_Port, XG_CAM_PWR_EN_Pin)
#define YT_CAM_PWR_ON() \
    HAL_GPIO_WritePin(YT_CAM_PWR_EN_GPIO_Port, YT_CAM_PWR_EN_Pin, GPIO_PIN_SET)
#define YT_CAM_PWR_OFF()                                          \
    HAL_GPIO_WritePin(YT_CAM_PWR_EN_GPIO_Port, YT_CAM_PWR_EN_Pin, \
                      GPIO_PIN_RESET)
#define YT_CAM_PWR_READ() \
    HAL_GPIO_ReadPin(YT_CAM_PWR_EN_GPIO_Port, YT_CAM_PWR_EN_Pin)
#define UITRASONIC_PWR_ON()                                               \
    HAL_GPIO_WritePin(UITRASONIC_PWR_EN_GPIO_Port, UITRASONIC_PWR_EN_Pin, \
                      GPIO_PIN_SET)
#define UITRASONIC_PWR_OFF()                                              \
    HAL_GPIO_WritePin(UITRASONIC_PWR_EN_GPIO_Port, UITRASONIC_PWR_EN_Pin, \
                      GPIO_PIN_RESET)
#define UITRASONIC_PWR_READ() \
    HAL_GPIO_ReadPin(UITRASONIC_PWR_EN_GPIO_Port, UITRASONIC_PWR_EN_Pin)
#define SYS_PWR_ON() \
    HAL_GPIO_WritePin(SYS_PWR_EN_GPIO_Port, SYS_PWR_EN_Pin, GPIO_PIN_SET)
#define SYS_PWR_OFF() \
    HAL_GPIO_WritePin(SYS_PWR_EN_GPIO_Port, SYS_PWR_EN_Pin, GPIO_PIN_RESET)
#define SYS_PWR_READ() HAL_GPIO_ReadPin(SYS_PWR_EN_GPIO_Port, SYS_PWR_EN_Pin)
#define E2PROM_WP_EN_ON() \
    HAL_GPIO_WritePin(E2PROM_WP_EN_GPIO_Port, E2PROM_WP_EN_Pin, GPIO_PIN_SET)
#define E2PROM_WP_EN_OFF() \
    HAL_GPIO_WritePin(E2PROM_WP_EN_GPIO_Port, E2PROM_WP_EN_Pin, GPIO_PIN_RESET)
#define DEBUG_USART_SW2_H()                                           \
    HAL_GPIO_WritePin(DEBUG_USART_SW2_GPIO_Port, DEBUG_USART_SW2_Pin, \
                      GPIO_PIN_SET)
#define DEBUG_USART_SW2_L()                                           \
    HAL_GPIO_WritePin(DEBUG_USART_SW2_GPIO_Port, DEBUG_USART_SW2_Pin, \
                      GPIO_PIN_RESET)
#define DEBUG_USART_SW1_H()                                           \
    HAL_GPIO_WritePin(DEBUG_USART_SW1_GPIO_Port, DEBUG_USART_SW1_Pin, \
                      GPIO_PIN_SET)
#define DEBUG_USART_SW1_L()                                           \
    HAL_GPIO_WritePin(DEBUG_USART_SW1_GPIO_Port, DEBUG_USART_SW1_Pin, \
                      GPIO_PIN_RESET)
#define POWER_KEY_READ() HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port, POWER_KEY_Pin)
#define MCU_LED_TOGGLE() HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin)
#define HEAT_CTL_ON() \
    HAL_GPIO_WritePin(HEAT_CTL_GPIO_Port, HEAT_CTL_Pin, GPIO_PIN_SET)
#define HEAT_CTL_OFF() \
    HAL_GPIO_WritePin(HEAT_CTL_GPIO_Port, HEAT_CTL_Pin, GPIO_PIN_RESET)
#define UITRASONIC_ECHO_DETECT_READ()                  \
    HAL_GPIO_ReadPin(UITRASONIC_ECHO_DETECT_GPIO_Port, \
                     UITRASONIC_ECHO_DETECT_Pin)

typedef enum {
    DEBUG_USART_EN_RK3308 = 0,
    DEBUG_USART_EN_RK1808_3,
    DEBUG_USART_EN_RK1808_1,
    DEBUG_USART_EN_RK1808_2,
} debug_usart_index_enum_t;

void drv_gpio_init(void);
void drv_gpio_debug_usart_sw_ctrl(debug_usart_index_enum_t index);

#endif
