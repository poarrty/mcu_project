#include "bsp_clean_fun.h"
#include "tim.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "cmsis_os.h"
#include "log.h"
#include <string.h>

add_water_var_stu_t add_water_var;
pump_pwm_stu_t pump_pwm;

/******************************************************************************
 * @Function: sewage_pump_start
 * @Description: 排污泵启动函数
 * @Input:  None
 * @Output: None
 * @Return: void
 * @Others: None
 * @param
 *******************************************************************************/
void sewage_pump_start(void) {
    HAL_StatusTypeDef status;
    LOG_DEBUG("%s:[0]", __FUNCTION__);

    if (pump_pwm.sewage_pump_status != PUMP_ON) {
        LOG_DEBUG("%s:[0k]", __FUNCTION__);
        pump_pwm.sewage_pump_status = PUMP_ON;
        pump_pwm.sewage_pump_pwm_pulse = 50;

        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,
                              pump_pwm.sewage_pump_pwm_pulse);

        status = HAL_TIM_Base_Start_IT(&htim4);

        if (status != HAL_OK) {
            LOG_DEBUG("%s:[%d]", __FUNCTION__, status);
        }

        status = HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);

        if (status != HAL_OK) {
            LOG_DEBUG("%s:[%d]", __FUNCTION__, status);
        }
    }
}

void sewage_pump_puase(void) {
    if (pump_pwm.sewage_pump_status != PUMP_OFF) {
        pump_pwm.sewage_pump_status = PUMP_OFF;
        pump_pwm.sewage_pump_pwm_pulse = 1;
    }

    CL_SEWAGE_PUMP_CTRL_TURN_OFF();
}

void sewage_pump_stop(void) {
    LOG_DEBUG("%s:[0]", __FUNCTION__);

    if (pump_pwm.sewage_pump_status != PUMP_OFF) {
        pump_pwm.sewage_pump_status = PUMP_OFF;
        pump_pwm.sewage_pump_pwm_pulse = 1;
    }

    CL_SEWAGE_PUMP_CTRL_TURN_OFF();
    SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
}

/******************************************************************************
 * @Function: sewage_pump_start
 * @Description: 加水泵启动函数
 * @Input:  None
 * @Output: None
 * @Return: void
 * @Others: None
 * @param
 *******************************************************************************/
void add_water_pump_start(void) {
    HAL_StatusTypeDef status;
    LOG_DEBUG("%s:1", __FUNCTION__);

    if (pump_pwm.add_water_pump_status != PUMP_ON) {
        LOG_DEBUG("%s:ok", __FUNCTION__);
        pump_pwm.add_water_pump_status = PUMP_ON;
        pump_pwm.add_water_pump_pwm_pulse = 50;

        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2,
                              pump_pwm.add_water_pump_pwm_pulse);

        status = HAL_TIM_Base_Start_IT(&htim4);

        if (status != HAL_OK) {
            LOG_DEBUG("%s:[%d]", __FUNCTION__, status);
        }

        status = HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_2);

        if (status != HAL_OK) {
            LOG_DEBUG("%s:[%d]", __FUNCTION__, status);
        }
    }
}

void add_water_pump_stop(void) {
    if (pump_pwm.add_water_pump_status != PUMP_OFF) {
        pump_pwm.add_water_pump_status = PUMP_OFF;
        pump_pwm.add_water_pump_pwm_pulse = 1;
    }

    CL_CLEAR_WATER_PUMP_CTRL_TURN_OFF();
}

void all_clean_fun_stop(void) {
    SYS_CLR_BIT(add_water_var.add_water_status, ADD_WATER);
    SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP);
    CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_OFF();
    //关闭加水电磁阀
    ADD_WATER_VALVE_CTRL_TURN_OFF();
    CL_TRANSITION_BOX_VALVE_CTRL_TURN_OFF();
    add_water_pump_stop();
    sewage_pump_stop();
}

void all_device_start(void) {
    ADD_WATER_VALVE_CTRL_TURN_ON();
    CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_ON();
    CL_TRANSITION_BOX_VALVE_CTRL_TURN_ON();
    osDelay(1000);

    SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
    osDelay(1000);
    osDelay(1000);
    SYS_SET_BIT(add_water_var.add_water_status, ADD_WATER_PUMP_START_FLAG);
    SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP);
}