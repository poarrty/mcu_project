/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_gpio.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:21:56
 * @Description: GPIO底层接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "main.h"

#define CHARGER_OUTPUT_TURN_ON()                                          \
    HAL_GPIO_WritePin(CHARGER_OUTPUT_EN_GPIO_Port, CHARGER_OUTPUT_EN_Pin, \
                      GPIO_PIN_SET)
#define CHARGER_OUTPUT_TURN_OFF()                                         \
    HAL_GPIO_WritePin(CHARGER_OUTPUT_EN_GPIO_Port, CHARGER_OUTPUT_EN_Pin, \
                      GPIO_PIN_RESET)
#define CHARGER_OUTPUT_STATUS_READ() \
    HAL_GPIO_ReadPin(CHARGER_OUTPUT_EN_GPIO_Port, CHARGER_OUTPUT_EN_Pin)

#define WIRE_CHARGE_OUTPUT_TURN_ON()                   \
    HAL_GPIO_WritePin(WIRE_CHARGE_OUTPUT_EN_GPIO_Port, \
                      WIRE_CHARGE_OUTPUT_EN_Pin, GPIO_PIN_SET)
#define WIRE_CHARGE_OUTPUT_TURN_OFF()                  \
    HAL_GPIO_WritePin(WIRE_CHARGE_OUTPUT_EN_GPIO_Port, \
                      WIRE_CHARGE_OUTPUT_EN_Pin, GPIO_PIN_RESET)
#define WIRE_CHARGE_OUTPUT_STATUS_READ() \
    HAL_GPIO_ReadPin(WIRE_CHARGE_OUTPUT_EN_GPIO_Port, WIRE_CHARGE_OUTPUT_EN_Pin)

#define AUTO_CHARGE_OUTPUT_TURN_ON()                   \
    HAL_GPIO_WritePin(AUTO_CHARGE_OUTPUT_EN_GPIO_Port, \
                      AUTO_CHARGE_OUTPUT_EN_Pin, GPIO_PIN_SET)
#define AUTO_CHARGE_OUTPUT_TURN_OFF()                  \
    HAL_GPIO_WritePin(AUTO_CHARGE_OUTPUT_EN_GPIO_Port, \
                      AUTO_CHARGE_OUTPUT_EN_Pin, GPIO_PIN_RESET)
#define AUTO_CHARGE_OUTPUT_STATUS_READ() \
    HAL_GPIO_ReadPin(AUTO_CHARGE_OUTPUT_EN_GPIO_Port, AUTO_CHARGE_OUTPUT_EN_Pin)

#define STOP_CHARGE_CTRL_KEY_READ() \
    HAL_GPIO_ReadPin(STOP_CHARGE_CTRL_KEY_GPIO_Port, STOP_CHARGE_CTRL_KEY_Pin)

#define CHARGE_ELECTRODE_STATUS_KEY_READ()                  \
    HAL_GPIO_ReadPin(CHARGE_ELECTRODE_STATUS_KEY_GPIO_Port, \
                     CHARGE_ELECTRODE_STATUS_KEY_Pin)

#define CHARGER_FORCE_OUTPUT_CTRL_KEY_READ()             \
    HAL_GPIO_ReadPin(CHARGER_FORCE_OUTPUT_KEY_GPIO_Port, \
                     CHARGER_FORCE_OUTPUT_KEY_Pin)

#define DIGITAL_TUBE_LOCK_H()                         \
    HAL_GPIO_WritePin(DIGITAL_TUBE_LOCK_EN_GPIO_Port, \
                      DIGITAL_TUBE_LOCK_EN_Pin, GPIO_PIN_SET)
#define DIGITAL_TUBE_LOCK_L()                         \
    HAL_GPIO_WritePin(DIGITAL_TUBE_LOCK_EN_GPIO_Port, \
                      DIGITAL_TUBE_LOCK_EN_Pin, GPIO_PIN_RESET)

#define DIGITAL_TUBE_PWR_TURN_ON()                                            \
    HAL_GPIO_WritePin(DIGITAL_TUBE_PWR_EN_GPIO_Port, DIGITAL_TUBE_PWR_EN_Pin, \
                      GPIO_PIN_SET)
#define DIGITAL_TUBE_PWR_TURN_OFF()                                           \
    HAL_GPIO_WritePin(DIGITAL_TUBE_PWR_EN_GPIO_Port, DIGITAL_TUBE_PWR_EN_Pin, \
                      GPIO_PIN_RESET)

#define LED_G_TURN_ON() \
    HAL_GPIO_WritePin(LED_G_EN_GPIO_Port, LED_G_EN_Pin, GPIO_PIN_SET)
#define LED_G_TURN_OFF() \
    HAL_GPIO_WritePin(LED_G_EN_GPIO_Port, LED_G_EN_Pin, GPIO_PIN_RESET)
#define LED_G_TOGGLE() HAL_GPIO_TogglePin(LED_G_EN_GPIO_Port, LED_G_EN_Pin)

#define LED_R_TURN_ON() \
    HAL_GPIO_WritePin(LED_R_EN_GPIO_Port, LED_R_EN_Pin, GPIO_PIN_SET)
#define LED_R_TURN_OFF() \
    HAL_GPIO_WritePin(LED_R_EN_GPIO_Port, LED_R_EN_Pin, GPIO_PIN_RESET)
#define LED_R_TOGGLE() HAL_GPIO_TogglePin(LED_R_EN_GPIO_Port, LED_R_EN_Pin)

#define USART3_RS485_RECV_EN()                                        \
    HAL_GPIO_WritePin(USART3_RS485_DE_GPIO_Port, USART3_RS485_DE_Pin, \
                      GPIO_PIN_RESET)
#define USART3_RS485_SEND_EN()                                        \
    HAL_GPIO_WritePin(USART3_RS485_DE_GPIO_Port, USART3_RS485_DE_Pin, \
                      GPIO_PIN_SET)

#define E2PROM_WP_TURN_ON() \
    HAL_GPIO_WritePin(E2PROM_WP_EN_GPIO_Port, E2PROM_WP_EN_Pin, GPIO_PIN_SET)
#define E2PROM_WP_TURN_OFF() \
    HAL_GPIO_WritePin(E2PROM_WP_EN_GPIO_Port, E2PROM_WP_EN_Pin, GPIO_PIN_RESET)

#define ADD_WATER_VALVE_CTRL_TURN_ON()                      \
    HAL_GPIO_WritePin(CL_ADD_WATER_VALVE_CTRL_EN_GPIO_Port, \
                      CL_ADD_WATER_VALVE_CTRL_EN_Pin, GPIO_PIN_SET)
#define ADD_WATER_VALVE_CTRL_TURN_OFF()                     \
    HAL_GPIO_WritePin(CL_ADD_WATER_VALVE_CTRL_EN_GPIO_Port, \
                      CL_ADD_WATER_VALVE_CTRL_EN_Pin, GPIO_PIN_RESET)
#define ADD_WATER_VALVE_CTRL_STATUS_READ()                 \
    HAL_GPIO_ReadPin(CL_ADD_WATER_VALVE_CTRL_EN_GPIO_Port, \
                     CL_ADD_WATER_VALVE_CTRL_EN_Pin)

#define CL_TRANSITION_BOX_VALVE_CTRL_TURN_ON()                    \
    HAL_GPIO_WritePin(CL_TRANSITION_BOX_VALVE_CTRLL_EN_GPIO_Port, \
                      CL_TRANSITION_BOX_VALVE_CTRLL_EN_Pin, GPIO_PIN_SET)
#define CL_TRANSITION_BOX_VALVE_CTRL_TURN_OFF()                   \
    HAL_GPIO_WritePin(CL_TRANSITION_BOX_VALVE_CTRLL_EN_GPIO_Port, \
                      CL_TRANSITION_BOX_VALVE_CTRLL_EN_Pin, GPIO_PIN_RESET)
#define CL_TRANSITION_BOX_VALVE_CTRL_STATUS_READ()               \
    HAL_GPIO_ReadPin(CL_TRANSITION_BOX_VALVE_CTRLL_EN_GPIO_Port, \
                     CL_TRANSITION_BOX_VALVE_CTRLL_EN_Pin)

#define CL_SEWAGE_PUMP_CTRL_TURN_ON()                   \
    HAL_GPIO_WritePin(CL_SEWAGE_PUMP_CTRL_EN_GPIO_Port, \
                      CL_SEWAGE_PUMP_CTRL_EN_Pin, GPIO_PIN_SET)
#define CL_SEWAGE_PUMP_CTRL_TURN_OFF()                  \
    HAL_GPIO_WritePin(CL_SEWAGE_PUMP_CTRL_EN_GPIO_Port, \
                      CL_SEWAGE_PUMP_CTRL_EN_Pin, GPIO_PIN_RESET)
#define CL_SEWAGE_PUMP_CTRL_STATUS_READ()              \
    HAL_GPIO_ReadPin(CL_SEWAGE_PUMP_CTRL_EN_GPIO_Port, \
                     CL_SEWAGE_PUMP_CTRL_EN_Pin)

#define CL_CLEAR_WATER_PUMP_CTRL_TURN_ON()                   \
    HAL_GPIO_WritePin(CL_CLEAR_WATER_PUMP_CTRL_EN_GPIO_Port, \
                      CL_CLEAR_WATER_PUMP_CTRL_EN_Pin, GPIO_PIN_SET)
#define CL_CLEAR_WATER_PUMP_CTRL_TURN_OFF()                  \
    HAL_GPIO_WritePin(CL_CLEAR_WATER_PUMP_CTRL_EN_GPIO_Port, \
                      CL_CLEAR_WATER_PUMP_CTRL_EN_Pin, GPIO_PIN_RESET)
#define CL_CLEAR_WATER_PUMP_CTRL_STATUS_READ()              \
    HAL_GPIO_ReadPin(CL_CLEAR_WATER_PUMP_CTRL_EN_GPIO_Port, \
                     CL_CLEAR_WATER_PUMP_CTRL_EN_Pin)

#define CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_ON()                         \
    HAL_GPIO_WritePin(CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_GPIO_Port, \
                      CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_Pin,       \
                      GPIO_PIN_SET)
#define CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_OFF()                        \
    HAL_GPIO_WritePin(CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_GPIO_Port, \
                      CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_Pin,       \
                      GPIO_PIN_RESET)
#define CL_CLEANER_DIAPHRAGM_PUMP_CTRL_STATUS_READ()                    \
    HAL_GPIO_ReadPin(CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_GPIO_Port, \
                     CL_TRANSITION_BOX_DRAINAGE_PUMP_CTRL_EN_Pin)

#define CL_WATER_LEVEL_SENSOR_STATUS_KEY_READ()                     \
    HAL_GPIO_ReadPin(CL_WATER_LEVEL_SENSOR_STATUS_DETECT_GPIO_Port, \
                     CL_WATER_LEVEL_SENSOR_STATUS_DETECT_Pin)

void drv_gpio_L(void);
void drv_gpio_H(void);
void drv_gpio_auto_charge_set(FunctionalState state);
void drv_gpio_wire_charge_set(FunctionalState state);
void drv_gpio_charge_off(void);

#endif
