/******************************************************************************
 * Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_digital_tube.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-03-25 21:21:56
 * @Description: 数码管任务文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_digital_tube.h"
#include "fal_charge_auto.h"
#include "bsp_digital_tube.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "cmsis_os.h"
#include <string.h>

digital_tube_var_stu_t digital_tube_var;

/******************************************************************************
 * @Function: digital_tube_init
 * @Description: 数码管初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void digital_tube_init(void) {
    memset(&digital_tube_var, 0, sizeof(digital_tube_var_stu_t));
    DIGITAL_TUBE_PWR_TURN_ON();

#if __TEST_EN__
    SYS_SET_BIT(digital_tube_var.flag, DIGITAL_TUBE_DISPLAY_ON_EN_BIT);
    SYS_SET_BIT(digital_tube_var.flag, DIGITAL_TUBE_DYNAMIC_DISPLAY_EN_BIT);
    digital_tube_var.display_data = 55;
    digital_tube_var.progress_bar_level =
        bsp_get_digital_tube_progress_bar_level(digital_tube_var.display_data);
    DIGITAL_TUBE_PWR_TURN_ON();
#endif
}

/******************************************************************************
 * @Function: digital_tube_display_off
 * @Description: 关闭数码管显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void digital_tube_display_off(void) {
    memset(&digital_tube_var, 0, sizeof(digital_tube_var_stu_t));
}

/******************************************************************************
 * @Function: digital_tube_display_on
 * @Description: 打开数码管显示
 * @Input: val：要显示的值
 *         type：0：动态显示  1：静态显示
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} val
 * @param {digital_tube_display_type_enum_t} type
 *******************************************************************************/
void digital_tube_display_on(uint16_t val,
                             digital_tube_display_type_enum_t type,
                             digital_tube_display_mode_enum_t display_mode) {
    uint8_t progress_bar_level = 0;

    switch (type) {
        case DYNAMIC_DISPLAY:
            SYS_SET_BIT(digital_tube_var.flag,
                        DIGITAL_TUBE_DYNAMIC_DISPLAY_EN_BIT);
            break;

        case STATIC_DISPLAY:
            SYS_CLR_BIT(digital_tube_var.flag,
                        DIGITAL_TUBE_DYNAMIC_DISPLAY_EN_BIT);
            break;
    }

    if (display_mode == FUNCTION_CODE_DISPLAY) {
        digital_tube_var.display_data[0] = val;
        digital_tube_var.display_data_type[0] = FUNCTION_CODE_DISPLAY;
        digital_tube_var.display_data_num = 1;
    } else if (display_mode == BATTERY_DISPLAY) {
        //设置进度条起始位置
        digital_tube_var.display_data[0] = val;
        digital_tube_var.display_data_type[0] = BATTERY_DISPLAY;
        progress_bar_level = bsp_get_digital_tube_progress_bar_level(
            digital_tube_var.display_data[0]);
        digital_tube_var.progress_bar_level =
            (digital_tube_var.progress_bar_level_pre != progress_bar_level)
                ? progress_bar_level
                : digital_tube_var.progress_bar_level;
        digital_tube_var.progress_bar_level_pre = progress_bar_level;

        digital_tube_var.display_data_num = 1;
    } else {
        digital_tube_var.display_data[1] = val;
        digital_tube_var.display_data_type[1] = WATER_LEVEL_DISPLAY;

        digital_tube_var.display_data_num = 2;
    }

    //使能数码管显示
    SYS_SET_BIT(digital_tube_var.flag, DIGITAL_TUBE_DISPLAY_ON_EN_BIT);

    //打开数码管电源
    DIGITAL_TUBE_PWR_TURN_ON();

    //清除自动充电超时事件
    SYS_CLR_BIT(auto_charge_var.flag, CHARGE_AUTO_TIMEOUT_EN_BIT);
    auto_charge_var.timeout_cnt = 0;
}

/******************************************************************************
 * @Function: digital_tube_display_server
 * @Description: 数码管服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void digital_tube_display_server(void) {
    if (SYS_GET_BIT(digital_tube_var.flag, DIGITAL_TUBE_DISPLAY_ON_EN_BIT)) {
        bsp_digital_tube_send_data();
    } else if (SYS_GET_BIT(auto_charge_var.flag,
                           CHARGE_AUTO_ELECTRODE_DOWN_BIT)) {
        bsp_digital_tube_electrode_down_display();
    } else {
        bsp_digital_tube_standby_display();
    }
}

/******************************************************************************
 * @Function: task_digital_tube_init
 * @Description: 数码管任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_digital_tube_init(void) {
    digital_tube_init();

    return 0;
}

/******************************************************************************
 * @Function: task_digital_tube_deInit
 * @Description: 数码管任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_digital_tube_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_digital_tube_run
 * @Description: 数码管任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_digital_tube_run(void *pvParameters) {
    task_digital_tube_init();

    while (1) {
        digital_tube_display_server();

        osDelay(5);
        if (SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT)) {
            digital_tube_display_on(DIGITAL_TUBE_BUTTON_STOP, STATIC_DISPLAY,
                                    FUNCTION_CODE_DISPLAY);
        }
    }
}
