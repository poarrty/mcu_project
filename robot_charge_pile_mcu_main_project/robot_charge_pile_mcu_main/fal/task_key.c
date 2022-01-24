/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_key.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-09-14 11:25:17
 * @Description: 按键任务入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_key.h"
#include "task_charge.h"
#include "fal_charge_auto.h"
#include "task_led.h"
#include "fal_charge_wire.h"
#include "task_digital_tube.h"
#include "task_ir_38k.h"
#include "pal_ir_38k.h"
#include "task_test_mode.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "pal_uros.h"
#include "log.h"
#include "cmsis_os.h"
#include "bsp_clean_fun.h"
#include "fal_version.h"

struct Button stop_charge_ctrl_key;
struct Button charger_force_output_ctrl_key;
struct Button charge_electrode_status_key;
struct Button cl_water_level_sensor_status_key;

PressEvent stop_charge_ctrl_key_event[] = {PRESS_DOWN, PRESS_UP};
PressEvent charger_force_output_ctrl_key_event[] = {PRESS_DOWN};
PressEvent charge_electrode_status_key_event[] = {PRESS_DOWN, PRESS_UP};
PressEvent cl_water_level_sensor_status_key_event[] = {PRESS_DOWN, PRESS_UP};

static uint8_t enter_test_mode_status;

extern void electrode_up_publish(void);

button_info_stu_t button_info[BUTTON_INDEX_MAX] = {
    {&stop_charge_ctrl_key, read_stop_charge_ctrl_key_pin_level, ACTIVE_HIGH,
     ARRAY_SIZE(stop_charge_ctrl_key_event), stop_charge_ctrl_key_event,
     stop_charge_ctrl_key_event_callbake},

    {&charger_force_output_ctrl_key,
     read_charger_force_output_ctrl_key_pin_level, ACTIVE_LOW,
     ARRAY_SIZE(charger_force_output_ctrl_key_event),
     charger_force_output_ctrl_key_event,
     charger_force_output_ctrl_key_event_callbake},

    {&charge_electrode_status_key, read_charge_electrode_status_key_pin_level,
     ACTIVE_HIGH, ARRAY_SIZE(charge_electrode_status_key_event),
     charge_electrode_status_key_event,
     charge_electrode_status_key_event_callbake},

    {&cl_water_level_sensor_status_key,
     read_cl_water_level_sensor_status_key_pin_level, ACTIVE_LOW,
     ARRAY_SIZE(cl_water_level_sensor_status_key_event),
     cl_water_level_sensor_status_key_event,
     cl_water_level_sensor_status_key_event_callbake},
};

/******************************************************************************
 * @Function: read_stop_charge_ctrl_key_pin_level
 * @Description: 获取停止充电按键的电平状态
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t read_stop_charge_ctrl_key_pin_level(void) {
    return STOP_CHARGE_CTRL_KEY_READ();
}

/******************************************************************************
 * @Function: read_charger_force_output_ctrl_key_pin_level
 * @Description: 获取强充按键的电平状态
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t read_charger_force_output_ctrl_key_pin_level(void) {
    return CHARGER_FORCE_OUTPUT_CTRL_KEY_READ();
}

/******************************************************************************
 * @Function: read_charge_electrode_status_key_pin_level
 * @Description: 获取电极的电平状态
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t read_charge_electrode_status_key_pin_level(void) {
    return CHARGE_ELECTRODE_STATUS_KEY_READ();
}

/******************************************************************************
 * @Function: read_cl_water_level_sensor_status_key_pin_level
 * @Description: 获取水位传感器的电平状态
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t read_cl_water_level_sensor_status_key_pin_level(void) {
    return CL_WATER_LEVEL_SENSOR_STATUS_KEY_READ();
}

/******************************************************************************
 * @Function: stop_charge_ctrl_key_event_callbake
 * @Description: 停止充电按键事件回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *button
 *******************************************************************************/
void stop_charge_ctrl_key_event_callbake(void *button) {
    uint32_t button_event_val = 0;

    button_event_val = get_button_event((struct Button *) button);

    // LOG_DEBUG("%s:[%d]",__FUNCTION__,button_event_val);

    if (button_event_val == PRESS_DOWN) {
        SYS_SET_BIT(enter_test_mode_status, BUTTON_STOP_CHARGE);

        drv_gpio_charge_off();
        all_clean_fun_stop();
        digital_tube_var.progress_bar_level_pre = 0;
        SYS_CLR_BIT(wire_charge_var.flag, CHARGE_WIRE_KEY_TRIGGER_BIT);
        SYS_SET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT);
        SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);
        digital_tube_display_on(DIGITAL_TUBE_BUTTON_STOP, STATIC_DISPLAY,
                                FUNCTION_CODE_DISPLAY);

        LOG_DEBUG("Stop charge key down!");
    } else if (button_event_val == PRESS_UP) {
        SYS_CLR_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT);
        digital_tube_display_off();

        LOG_DEBUG("Stop charge key up!");
    }
}

/******************************************************************************
 * @Function: charger_force_output_ctrl_key_event_callbake
 * @Description: 强充按键事件回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *button
 *******************************************************************************/
void charger_force_output_ctrl_key_event_callbake(void *button) {
    uint32_t button_event_val = 0;

    button_event_val = get_button_event((struct Button *) button);

    if (button_event_val == PRESS_DOWN) {
        LOG_DEBUG("Force charge key down!");

        SYS_SET_BIT(enter_test_mode_status, BUTTON_FORCE_CHARGE);
        //手推强充功能需要先压下电极再按强充按钮
        if (SYS_GET_BIT(auto_charge_var.flag, CHARGE_AUTO_ELECTRODE_DOWN_BIT)) {
            if (!SYS_GET_BIT(wire_charge_var.flag,
                             CHARGE_WIRE_KEY_TRIGGER_BIT)) {
                if (!SYS_GET_BIT(wire_charge_var.flag,
                                 CHARGE_WIRE_COM_TRIGGER_BIT)) {
                    if (SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT)) {
                        //恢复充电
                        SYS_SET_BIT(auto_charge_var.flag,
                                    CHARGE_AUTO_ELECTRODE_DOWN_BIT);
                    } else {
                        drv_gpio_auto_charge_set(ENABLE);
                    }

                    SYS_CLR_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT);
                    LOG_DEBUG("Start force auto charge!");
                } else {
                    LOG_WARNING("Wire charge is already enable with CAN!");
                }
            } else {
                LOG_WARNING("Wire charge is already enable with key!");
            }
        } else {
            if (!SYS_GET_BIT(wire_charge_var.flag,
                             CHARGE_WIRE_KEY_TRIGGER_BIT)) {
                if ((!SYS_GET_BIT(wire_charge_var.flag,
                                  CHARGE_WIRE_COM_TRIGGER_BIT)) &&
                    (!SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT))) {
                    drv_gpio_wire_charge_set(ENABLE);
                    //标记按键触发有线充电
                    SYS_SET_BIT(wire_charge_var.flag,
                                CHARGE_WIRE_KEY_TRIGGER_BIT);
                    LOG_DEBUG("Start force wire charge!");
                } else {
                    LOG_WARNING("Wire charge is already enable with CAN!");
                }
            } else {
                LOG_WARNING("Wire charge is already enable with key!");
            }
        }
    }
}

/******************************************************************************
 * @Function: electrode_status_change_callbake
 * @Description: 电极状态改变处理函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {electrode_status_enum_t} status
 *******************************************************************************/
void electrode_status_change_callbake(electrode_status_enum_t status) {
    if (status == ELECTRODE_STATUS_DOWN) {
        LOG_DEBUG("Electrode down!");
        BT_POWER_ON;
        //电极压下，绿灯闪烁
        led_var.current_status = LED_STATUS_G_FLASH;

        if (!SYS_GET_BIT(wire_charge_var.flag, CHARGE_WIRE_KEY_TRIGGER_BIT)) {
            if (!SYS_GET_BIT(wire_charge_var.flag,
                             CHARGE_WIRE_COM_TRIGGER_BIT)) {
                SYS_SET_BIT(auto_charge_var.flag,
                            CHARGE_AUTO_ELECTRODE_DOWN_BIT);
                SYS_SET_BIT(ir_38k_var.flag, SEND_ELECTRODE_DOWN_BIT);

                auto_charge_var.electrode_down_send_times =
                    ELECTRODE_DOWN_SEND_TIMES;

                // ir_38k_var.num = 0;
                ir_38k_var.msg_type = MSG_TYPE_A0;
                ir_38k_var.msg = SEND_ELECTRODE_DOWN_CMD;
                ir_38k_var.send_times = 30;

                micro_connect_flag = 1;
                LOG_DEBUG("Start auto charge!");
            } else {
                LOG_WARNING("Wire charge is already enable with CAN!");
            }
        } else {
            LOG_WARNING("Wire charge is already enable with key!");
        }
    } else if (status == ELECTRODE_STATUS_UP) {
        LOG_DEBUG("Electrode up!");

        // publish ELECTRODE_UP publish
        SYS_SET_BIT(ir_38k_var.flag, SEND_ELECTRODE_UP_BIT);
        //电极弹起，绿灯常亮
        led_var.current_status = LED_STATUS_G_TURN_ON;
        SYS_CLR_BIT(auto_charge_var.flag, CHARGE_AUTO_ELECTRODE_DOWN_BIT);
        SYS_SET_BIT(auto_charge_var.flag, PUBLISH_ELECTRODE_UP_BIT);
        SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);

        //清除充电检测超时
        SYS_CLR_BIT(auto_charge_var.flag, CHARGE_AUTO_TIMEOUT_EN_BIT);
        auto_charge_var.timeout_cnt = 0;
        drv_gpio_auto_charge_set(DISABLE);
        //清除停止充电标志

        all_clean_fun_stop();

        digital_tube_display_off();
        // electrode_up_publish();//需要在micro_connect_flag = 0;之前才能生效
        micro_connect_flag = 0;

        ir_38k_var.msg_type = MSG_TYPE_LOC;
        ir_38k_var.msg = 0;
        BT_POWER_OFF;
        // SYS_CLR_BIT(ir_38k_var.flag, IR_38K_RECEIVE_ENABLE);
    }
}

/******************************************************************************
 * @Function: charge_electrode_status_key_event_callbake
 * @Description: 电极按键事件回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *button
 *******************************************************************************/
void charge_electrode_status_key_event_callbake(void *button) {
    uint32_t button_event_val = 0;

    button_event_val = get_button_event((struct Button *) button);

    if (button_event_val == PRESS_DOWN) {
        // if (!SYS_GET_BIT(auto_charge_var.flag,
        // CHARGE_AUTO_ELECTRODE_DOWN_BIT))
        // {
        electrode_status_change_callbake(ELECTRODE_STATUS_DOWN);
        // }
    } else if (button_event_val == PRESS_UP) {
        // if ((SYS_GET_BIT(auto_charge_var.flag,
        // CHARGE_AUTO_ELECTRODE_DOWN_BIT)))
        // {
        electrode_status_change_callbake(ELECTRODE_STATUS_UP);
        // }
    }
}

/******************************************************************************
 * @Function: cl_water_level_sensor_status_key_event_callbake
 * @Description: 水位传感器按键事件回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *button
 *******************************************************************************/
void cl_water_level_sensor_status_key_event_callbake(void *button) {
    uint32_t button_event_val = 0;

    button_event_val = get_button_event((struct Button *) button);

    if (button_event_val == PRESS_DOWN) {
        SYS_SET_BIT(auto_charge_var.flag, CL_WATER_LEVEL_FULL_BIT);
    } else if (button_event_val == PRESS_UP) {
        SYS_CLR_BIT(auto_charge_var.flag, CL_WATER_LEVEL_FULL_BIT);
    }
}

/******************************************************************************
 * @Function: key_init
 * @Description: 按键初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void key_init(void) {
    uint8_t i = 0;
    uint8_t j = 0;

    for (i = 0; i < BUTTON_INDEX_MAX; i++) {
        button_init(button_info[i].button, button_info[i].read_button_pin_level,
                    button_info[i].tigger_level);

        for (j = 0; j < button_info[i].event_cnt; j++) {
            button_attach(button_info[i].button, button_info[i].event[j],
                          button_info[i].callbake);
        }

        button_start(button_info[i].button);
    }

    if (CHARGE_ELECTRODE_STATUS_KEY_READ()) {
        electrode_status_change_callbake(ELECTRODE_STATUS_DOWN);
    }
}

/******************************************************************************
 * @Function: key_server
 * @Description: 按键服务
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void key_server(void) {
    button_ticks();
}

/******************************************************************************
 * @Function: task_key_init
 * @Description: 按键任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_key_init(void) {
    key_init();

    return 0;
}

/******************************************************************************
 * @Function: task_key_deInit
 * @Description: 按键任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_key_deInit(void) {
    return 0;
}

void enter_test_mode_check(void) {
    for (uint8_t i = 5; i > 0; i--) {
        key_server();
        osDelay(10);
    }

    if (enter_test_mode_status == BUTTON_TEST_MODE_ENTER)  //测试模式
    {
        set_test_mode();
    }
}

/******************************************************************************
 * @Function: task_key_run
 * @Description: 按键任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_key_run(void *pvParameters) {
    task_key_init();
    enter_test_mode_check();

    while (1) {
        key_server();
        osDelay(5);
    }
}
