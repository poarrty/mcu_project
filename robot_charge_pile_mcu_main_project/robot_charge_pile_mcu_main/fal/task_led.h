/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_led.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description: LED任务入口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_LED_H__
#define __TASK_LED_H__

#include <stdint.h>

typedef enum {
    LED_STATUS_G_TURN_ON = 0,
    LED_STATUS_R_TURN_ON,
    LED_STATUS_G_FLASH,
    LED_STATUS_R_FLASH,
    LED_STATUS_R_G_FLASH,
    LED_STATUS_R_G_TURN_OFF,
} led_status_enum_t;

typedef enum {
    LED_R_G_FLASH_BIT = 0,
} led_flag_bit_enum_t;

typedef struct {
    // LED相关标志位
    uint8_t flag;
    // LED当前显示状态
    led_status_enum_t current_status;
} led_var_stu_t;

extern led_var_stu_t led_var;

void led_init(void);
void led_server(void);
int task_led_init(void);
int task_led_deInit(void);

#endif
