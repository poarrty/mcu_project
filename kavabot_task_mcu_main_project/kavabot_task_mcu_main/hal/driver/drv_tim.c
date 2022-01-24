/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 19:08:39
 * @Description: TIM中断回调接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_tim.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "bsp_s09.h"
#include "pal_usart3.h"
#include "fal_power.h"
#include "log.h"
#include "mb.h"
#include "mbport.h"

extern void prvvTIMERExpiredISR(void);

/******************************************************************************
 * @Function: drv_tim_init
 * @Description: TIM底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim_init(void) {
    Error_Check_Callbake(HAL_TIM_Base_Start_IT(&htim1));
}

/******************************************************************************
 * @Function: drv_tim1_update_irq_callbake
 * @Description: TIM1更新中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: 定时5ms
 *******************************************************************************/
void drv_tim1_update_irq_callbake(void) {
    if (SYS_GET_BIT(power_var.flag, POWER_ON_REQUEST_EN_BIT)) {
        power_var.power_on_respond_wait_timeout_cnt++;

        if (power_var.power_on_respond_wait_timeout_cnt ==
            POWER_ON_RESPOND_WAIT_TIMEOUT_CNT) {
            LOG_WARNING("Sys power on timeout!");
            SYS_CLR_BIT(power_var.flag, POWER_ON_REQUEST_EN_BIT);
            power_var.power_on_respond_wait_timeout_cnt = 0;
            pal_usart3_send_power_manager_respond_msg(
                PAL_USART3_CMD_TYPE_POWER_ON,
                PAL_USART3_POWER_MANAGER_RESPOND_TYPE_TIMEOUT);
        }
    } else {
        power_var.power_on_respond_wait_timeout_cnt = 0;
    }

    if (SYS_GET_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT)) {
        power_var.power_off_respond_wait_timeout_cnt++;

        if (power_var.power_off_respond_wait_timeout_cnt ==
            POWER_OFF_RESPOND_WAIT_TIMEOUT_CNT) {
            LOG_WARNING("Sys power off timeout!");
            fal_sys_power_off();
            SYS_CLR_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT);
            power_var.power_off_respond_wait_timeout_cnt = 0;
            pal_usart3_send_power_manager_respond_msg(
                PAL_USART3_CMD_TYPE_POWER_OFF,
                PAL_USART3_POWER_MANAGER_RESPOND_TYPE_TIMEOUT);
        }
    } else {
        power_var.power_off_respond_wait_timeout_cnt = 0;
    }

    eMBPoll();
}

/******************************************************************************
 * @Function: drv_tim3_update_irq_callbake
 * @Description: TIM3更新中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim3_update_irq_callbake(void) {
    prvvTIMERExpiredISR();
}

/******************************************************************************
 * @Function: drv_tim4_update_irq_callbake
 * @Description: TIM4更新中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim4_update_irq_callbake(void) {
    switch (s09_var.send_pulse_status) {
        case 0:
            s09_var.send_pulse_cnt++;
            if (s09_var.send_pulse_cnt == 4) {
                s09_var.send_pulse_cnt = 0;
                UITRASONIC_40KHZ_PULSE_OUTPUT_L();
                s09_var.send_pulse_status++;
            }
            break;

        case 1:
            UITRASONIC_40KHZ_PULSE_OUTPUT_H();
            s09_var.send_pulse_status++;
            break;

        case 2:
            UITRASONIC_40KHZ_PULSE_OUTPUT_L();
            s09_var.send_pulse_status++;
            break;

        case 3:
            s09_var.send_pulse_cnt++;
            if (s09_var.send_pulse_cnt == 2) {
                s09_var.send_pulse_cnt = 0;
                UITRASONIC_40KHZ_PULSE_OUTPUT_H();
                s09_var.send_pulse_status++;
            }
            break;

        case 4:
            s09_var.send_pulse_cnt++;
            if (s09_var.send_pulse_cnt == 2) {
                s09_var.send_pulse_cnt = 0;
                UITRASONIC_40KHZ_PULSE_OUTPUT_L();
                s09_var.send_pulse_status++;
            }
            break;

        case 5:
            s09_var.send_pulse_status++;
            //开启输入捕获
            Error_Check_Callbake(HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1));
            Error_Check_Callbake(HAL_TIM_Base_Start_IT(&htim5));
            __HAL_TIM_SET_COUNTER(&htim5, 0);
            break;

        default:
            break;
    }
}

/******************************************************************************
 * @Function: drv_tim5_update_irq_callbake
 * @Description: TIM5更新中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim5_update_irq_callbake(void) {
    SYS_SET_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_TIMEOUT_BIT);
    //关闭输入捕获
    Error_Check_Callbake(HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1));
    Error_Check_Callbake(HAL_TIM_Base_Stop_IT(&htim5));
}

/******************************************************************************
 * @Function: drv_tim5_ic_irq_callbake
 * @Description: TIM5输入捕获中断
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim5_ic_irq_callbake(void) {
    if (!UITRASONIC_ECHO_DETECT_READ()) {
        if (!SYS_GET_BIT(s09_var.flag, UITRASONIC_TIM_IC_FLAG_BIT)) {
            __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 0);
            SYS_SET_BIT(s09_var.flag, UITRASONIC_TIM_IC_FLAG_BIT);
        } else {
            s09_var.echo_time[s09_var.echo_cnt++] =
                __HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_1);
            if (s09_var.echo_cnt == UITRASONIC_ECHO_NUM) {
                s09_var.echo_cnt = 0;
                SYS_SET_BIT(s09_var.flag, UITRASONIC_RECV_ECHO_FINISH_BIT);
                //关闭输入捕获
                Error_Check_Callbake(HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1));
                Error_Check_Callbake(HAL_TIM_Base_Stop_IT(&htim5));
            }
            SYS_CLR_BIT(s09_var.flag, UITRASONIC_TIM_IC_FLAG_BIT);
        }
    }
}

/******************************************************************************
 * @Function: HAL_TIM_PeriodElapsedCallback
 * @Description: TIM更新中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        drv_tim1_update_irq_callbake();
    } else if (htim->Instance == TIM3) {
        drv_tim3_update_irq_callbake();
    } else if (htim->Instance == TIM4) {
        drv_tim4_update_irq_callbake();
    } else if (htim->Instance == TIM5) {
        drv_tim5_update_irq_callbake();
    }
}

/******************************************************************************
 * @Function: HAL_TIM_IC_CaptureCallback
 * @Description: 定时器输入捕获中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM5) {
        drv_tim5_ic_irq_callbake();
    }
}
