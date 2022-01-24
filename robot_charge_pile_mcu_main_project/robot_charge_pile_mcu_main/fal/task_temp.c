
/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_temp.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description: 温度传感器任务入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_temp.h"
#include "task_led.h"
#include "fal_charge_auto.h"
#include "fal_charge_wire.h"
#include "drv_gpio.h"
#include "log.h"
#include "cmsis_os.h"
#include <string.h>

temp_var_stu_t temp_var;

/******************************************************************************
 * @Function: temp_init
 * @Description: 温度传感器初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void temp_init(void) {
    memset(&temp_var, 0, sizeof(temp_var_stu_t));
}

/******************************************************************************
 * @Function: over_temp_protect_detect
 * @Description: 过温保护检测
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void over_temp_protect_detect(void) {
    bsp_get_temp_val(&temp_var.info);

    //超过过温保护阈值上限
    if (temp_var.info.T1 > OVER_PROTECT_THRESHOLD_UPPER_LIMIT) {
        LOG_WARNING("Over temp protect enable!, Current temp: %lf",
                    temp_var.info.T1);
        SYS_SET_BIT(temp_var.flag, OVER_PROTECT_EN_BIT);
        drv_gpio_charge_off();
        //过温保护使能后状态灯红灯常亮
        led_var.current_status = LED_STATUS_R_TURN_ON;
    }

    //低于过温保护恢复阈值下限
    if (temp_var.info.T1 < OVER_PROTECT_THRESHOLD_LOWER_LIMIT) {
        if (SYS_GET_BIT(temp_var.flag, OVER_PROTECT_EN_BIT)) {
            LOG_DEBUG("Over temp protect remove!, Current temp: %lf",
                      temp_var.info.T1);
            SYS_CLR_BIT(temp_var.flag, OVER_PROTECT_EN_BIT);
            //过温保护解除后状态灯恢复绿灯常亮
            led_var.current_status = LED_STATUS_G_TURN_ON;

            //恢复自动充电
            if (SYS_GET_BIT(auto_charge_var.flag,
                            CHARGE_AUTO_ELECTRODE_DOWN_BIT)) {
                SYS_SET_BIT(auto_charge_var.flag,
                            CHARGE_AUTO_ELECTRODE_DOWN_BIT);
            } else {
                //恢复有线充电
                if (SYS_GET_BIT(wire_charge_var.flag,
                                CHARGE_WIRE_KEY_TRIGGER_BIT) ||
                    SYS_GET_BIT(wire_charge_var.flag,
                                CHARGE_WIRE_COM_TRIGGER_BIT)) {
                    drv_gpio_wire_charge_set(ENABLE);
                }
            }
        }
    }
}

/******************************************************************************
 * @Function: task_temp_init
 * @Description: 温度传感器任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_temp_init(void) {
    temp_init();

    return 0;
}

/******************************************************************************
 * @Function: task_temp_deInit
 * @Description: 温度传感器任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_temp_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_temp_run
 * @Description: 温度传感器任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_temp_run(void *pvParameters) {
    task_temp_init();

    while (1) {
        over_temp_protect_detect();

        osDelay(1000);
    }
}
