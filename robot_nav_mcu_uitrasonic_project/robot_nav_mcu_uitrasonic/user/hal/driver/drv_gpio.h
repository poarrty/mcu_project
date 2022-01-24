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
#define STATE_LED_TOGGLE() \
    HAL_GPIO_TogglePin(STATE_LED_GPIO_Port, STATE_LED_Pin);
#define RS485_USART4_DE_H()                                           \
    HAL_GPIO_WritePin(RS485_USART4_DE_GPIO_Port, RS485_USART4_DE_Pin, \
                      GPIO_PIN_SET)
#define RS485_USART4_DE_L()                                           \
    HAL_GPIO_WritePin(RS485_USART4_DE_GPIO_Port, RS485_USART4_DE_Pin, \
                      GPIO_PIN_RESET)
#define IRDA_SD_EN_L() \
    HAL_GPIO_WritePin(IRDA_SD_EN_GPIO_Port, IRDA_SD_EN_Pin, GPIO_PIN_RESET)
#define IRDA_SD_EN_H() \
    HAL_GPIO_WritePin(IRDA_SD_EN_GPIO_Port, IRDA_SD_EN_Pin, GPIO_PIN_SET)

#endif
