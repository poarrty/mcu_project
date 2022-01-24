/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_gpio.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:21:56
 * @Description: GPIO底层接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_gpio.h"
#include "shell.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_gpio_L
 * @Description: 拉低所有GPIO
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_gpio_L(void) {
    CHARGER_OUTPUT_TURN_OFF();                  // PB15
    WIRE_CHARGE_OUTPUT_TURN_ON();               // PC12
    AUTO_CHARGE_OUTPUT_TURN_ON();               // PC0
    DIGITAL_TUBE_LOCK_L();                      // PA6
    DIGITAL_TUBE_PWR_TURN_OFF();                // PA15
    LED_G_TURN_OFF();                           // PC5
    LED_R_TURN_OFF();                           // PB5
    USART3_RS485_RECV_EN();                     // PB0
    E2PROM_WP_TURN_OFF();                       // PB1
    ADD_WATER_VALVE_CTRL_TURN_OFF();            // PC13
    CL_TRANSITION_BOX_VALVE_CTRL_TURN_OFF();    // PC14
    CL_SEWAGE_PUMP_CTRL_TURN_OFF();             // PC14
    CL_CLEAR_WATER_PUMP_CTRL_TURN_OFF();        // PC15
    CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_OFF();  // PB4
}

/******************************************************************************
 * @Function: drv_gpio_H
 * @Description: 拉高所有GPIO
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_gpio_H(void) {
    CHARGER_OUTPUT_TURN_ON();
    WIRE_CHARGE_OUTPUT_TURN_OFF();
    AUTO_CHARGE_OUTPUT_TURN_OFF();
    DIGITAL_TUBE_LOCK_H();
    DIGITAL_TUBE_PWR_TURN_ON();
    LED_G_TURN_ON();
    LED_R_TURN_ON();
    USART3_RS485_SEND_EN();
    E2PROM_WP_TURN_ON();
    ADD_WATER_VALVE_CTRL_TURN_ON();
    CL_TRANSITION_BOX_VALVE_CTRL_TURN_ON();
    CL_SEWAGE_PUMP_CTRL_TURN_ON();
    CL_CLEAR_WATER_PUMP_CTRL_TURN_ON();
    CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_ON();
}

/******************************************************************************
 * @Function: drv_gpio_auto_charge_set
 * @Description: 自动充电开关控制
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {FunctionalState} state
 *******************************************************************************/
void drv_gpio_auto_charge_set(FunctionalState state) {
    if (state == ENABLE) {
        AUTO_CHARGE_OUTPUT_TURN_ON();
        CHARGER_OUTPUT_TURN_ON();
    }

    if (state == DISABLE) {
        CHARGER_OUTPUT_TURN_OFF();
        AUTO_CHARGE_OUTPUT_TURN_OFF();
    }
}

/******************************************************************************
 * @Function: drv_gpio_wire_charge_set
 * @Description: 有线充电开关控制
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {FunctionalState} state
 *******************************************************************************/
void drv_gpio_wire_charge_set(FunctionalState state) {
    if (state == ENABLE) {
        WIRE_CHARGE_OUTPUT_TURN_ON();
        CHARGER_OUTPUT_TURN_ON();
    }

    if (state == DISABLE) {
        CHARGER_OUTPUT_TURN_OFF();
        WIRE_CHARGE_OUTPUT_TURN_OFF();
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 wire_charge_set, drv_gpio_wire_charge_set, wire charge set);

/******************************************************************************
 * @Function: drv_gpio_charge_off
 * @Description: 充电关闭
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_gpio_charge_off(void) {
    CHARGER_OUTPUT_TURN_OFF();
    AUTO_CHARGE_OUTPUT_TURN_OFF();
    WIRE_CHARGE_OUTPUT_TURN_OFF();
}
