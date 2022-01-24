/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:58:20
 * @Description: TIM中断回调接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_charge_auto.h"
#include "fal_charge_wire.h"
#include "task_led.h"
#include "task_battery.h"
#include "task_digital_tube.h"
#include "common_def.h"
#include "drv_gpio.h"
#include "bsp_clean_fun.h"
#include "bsp_digital_tube.h"
#include "log.h"
#include "tim.h"
#include <string.h>
#include "bsp_simuart_tx.h"

/******************************************************************************
 * @Function: drv_tim_init
 * @Description: TIM底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 *******************************************************************************/
HAL_StatusTypeDef drv_tim_init(void) {
    HAL_StatusTypeDef status;

    status = HAL_TIM_Base_Start_IT(&htim1);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}

/******************************************************************************
 * @Function: drv_tim1_update_irq_callbake
 * @Description: TIM1更新中断回调函数
 * @Input: 传入的TIM句柄
 * @Output: None
 * @Return: void
 * @Others: 定时周期5ms，用于基本定时
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void drv_tim1_update_irq_callbake(TIM_HandleTypeDef *htim) {
    //有线充电获取电池信息超时机制
    if (SYS_GET_BIT(battery_var.flag, BATTERY_INFO_REQUEST_UPDATE_BIT)) {
        battery_var.battery_info_timeout_cnt++;

        if (battery_var.battery_info_timeout_cnt ==
            BATTERY_INFO_REQUEST_TIMEOUT_CNT) {
            battery_var.battery_info_timeout_cnt = 0;
            SYS_CLR_BIT(battery_var.flag, BATTERY_INFO_REQUEST_UPDATE_BIT);
            SYS_CLR_BIT(wire_charge_var.flag, CHARGE_WIRE_COM_TRIGGER_BIT);
            memset(&battery_var.battery_info, 0, sizeof(battery_info_stu_t));
            digital_tube_display_off();
            drv_gpio_auto_charge_set(DISABLE);
            // CAN通信超时状态灯红灯常亮
            led_var.current_status = LED_STATUS_R_TURN_ON;
            LOG_WARNING("Battery CAN com timeout!");
        }
    }

    //自动充电超时机制
    if (SYS_GET_BIT(auto_charge_var.flag, CHARGE_AUTO_TIMEOUT_EN_BIT)) {
        auto_charge_var.timeout_cnt++;

        if (auto_charge_var.timeout_cnt == CHARGE_AUTO_TIMEOUT_CNT) {
            auto_charge_var.timeout_cnt = 0;
            led_var.current_status = LED_STATUS_R_TURN_ON;
            SYS_CLR_BIT(auto_charge_var.flag, CHARGE_AUTO_TIMEOUT_EN_BIT);
            LOG_WARNING("Wait charging timeout!");
        }
    }
}

/******************************************************************************
 * @Function: drv_tim3_update_irq_callbake
 * @Description: TIM1更新中断回调函数
 * @Input: 传入的TIM句柄
 * @Output: None
 * @Return: void
 * @Others: 定时周期1ms，加水泵，排污泵PWM
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/

void drv_tim4_update_irq_callbake(TIM_HandleTypeDef *htim) {
    if (pump_pwm.sewage_pump_status == PUMP_ON) {
        CL_SEWAGE_PUMP_CTRL_TURN_ON();
        if (pump_pwm.sewage_pump_pwm_pulse == MAX_PUMP_PWM_VALUE) {}
    }

    if (pump_pwm.add_water_pump_status == PUMP_ON) {
        CL_CLEAR_WATER_PUMP_CTRL_TURN_ON();

        if (pump_pwm.add_water_pump_pwm_pulse == MAX_PUMP_PWM_VALUE) {}
    }
}

/******************************************************************************
 * @Function: HAL_TIM_PeriodElapsedCallback
 * @Description: TIM更新中断回调函数
 * @Input: 传入的TIM句柄
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void MX_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    extern SimUart_Tx_TypeDef simuart_tx1;
    extern SimUart_Tx_TypeDef simuart_tx2;

    if (htim->Instance == TIM1) {
        drv_tim1_update_irq_callbake(htim);
    } else if (htim->Instance == TIM4) {
        drv_tim4_update_irq_callbake(htim);
    } else if (htim->Instance == TIM6) {
        bsp_simuart_tx_tim_handler(&simuart_tx1);
    } else if (htim->Instance == TIM7) {
        bsp_simuart_tx_tim_handler(&simuart_tx2);
    }
}

/******************************************************************************
 * @Function: HAL_TIM_OC_DelayElapsedCallback
 * @Description: PWM
 * @Input:  None
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            TIM4->CCR1 = pump_pwm.sewage_pump_pwm_pulse++;

            if (pump_pwm.sewage_pump_pwm_pulse == MAX_PUMP_PWM_VALUE) {
                //					pump_pwm.sewage_pump_pwm_pulse
                //=0;
                HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
                HAL_TIM_Base_Stop_IT(&htim4);
                CL_SEWAGE_PUMP_CTRL_TURN_ON();

            } else {
                CL_SEWAGE_PUMP_CTRL_TURN_OFF();
            }
        }
#if 1

        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            TIM4->CCR2 = pump_pwm.add_water_pump_pwm_pulse++;

            if (pump_pwm.add_water_pump_pwm_pulse == MAX_PUMP_PWM_VALUE) {
                //					pump_pwm.add_water_pump_pwm_pulse
                //= 0;
                HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2);
                HAL_TIM_Base_Stop_IT(&htim4);
                CL_CLEAR_WATER_PUMP_CTRL_TURN_ON();
            } else {
                CL_CLEAR_WATER_PUMP_CTRL_TURN_OFF();
            }
        }
#endif
    }
}
