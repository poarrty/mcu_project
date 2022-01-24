/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_key.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-09-14 11:24:35
 * @Description: 按键任务头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_KEY_H__
#define __TASK_KEY_H__

#include "multi_button.h"
#include <stdint.h>

typedef enum {
    ELECTRODE_STATUS_DOWN = 0,
    ELECTRODE_STATUS_UP = 1,
} electrode_status_enum_t;

typedef enum {
    STOP_CHARGE_CTRL_KEY_INDEX = 0,
    CHARGER_FORCE_OUTPUT_CTRL_KEY_INDEX,
    CHARGE_ELECTRODE_STATUS_KEY_INDEX,
    CL_WATER_LEVEL_SENSOR_STATUS_KEY_INDEX,
    BUTTON_INDEX_MAX,
} button_index_enum_t;

typedef enum {
    ACTIVE_LOW = 0,
    ACTIVE_HIGH = 1,
} button_trigger_level_enum_t;

typedef struct {
    struct Button *button;
    uint8_t (*read_button_pin_level)(void);
    button_trigger_level_enum_t tigger_level;
    uint8_t event_cnt;
    PressEvent *event;
    void (*callbake)(void *);
} button_info_stu_t;

void electrode_status_change_callbake(electrode_status_enum_t status);
uint8_t read_stop_charge_ctrl_key_pin_level(void);
uint8_t read_charger_force_output_ctrl_key_pin_level(void);
uint8_t read_charge_electrode_status_key_pin_level(void);
uint8_t read_cl_water_level_sensor_status_key_pin_level(void);
void stop_charge_ctrl_key_event_callbake(void *button);
void charger_force_output_ctrl_key_event_callbake(void *button);
void charge_electrode_status_key_event_callbake(void *button);
void cl_water_level_sensor_status_key_event_callbake(void *button);

#endif