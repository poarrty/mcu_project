/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_led.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description: LED任务入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_led.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "cmsis_os.h"
#include <string.h>

led_var_stu_t led_var;

/******************************************************************************
 * @Function: led_init
 * @Description: LED初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_init(void) {
    memset(&led_var, 0, sizeof(led_var_stu_t));
}

/******************************************************************************
 * @Function: led_R_turn_on
 * @Description: 红灯常亮
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_R_turn_on(void) {
    LED_G_TURN_OFF();
    LED_R_TURN_ON();
}

/******************************************************************************
 * @Function: led_G_turn_on
 * @Description: 绿灯常亮
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_G_turn_on(void) {
    LED_R_TURN_OFF();
    LED_G_TURN_ON();
}

/******************************************************************************
 * @Function: led_R_flash
 * @Description: 红灯闪烁
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_R_flash(void) {
    LED_G_TURN_OFF();
    LED_R_TOGGLE();
}

/******************************************************************************
 * @Function: led_G_flash
 * @Description: 绿灯闪烁
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_G_flash(void) {
    LED_R_TURN_OFF();
    LED_G_TOGGLE();
}

/******************************************************************************
 * @Function: led_R_G_flash
 * @Description: 红绿交替闪烁
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_R_G_flash(void) {
    if (!SYS_GET_BIT(led_var.flag, LED_R_G_FLASH_BIT)) {
        led_R_turn_on();
        SYS_SET_BIT(led_var.flag, LED_R_G_FLASH_BIT);
        return;
    }

    if (SYS_GET_BIT(led_var.flag, LED_R_G_FLASH_BIT)) {
        led_G_turn_on();
        SYS_CLR_BIT(led_var.flag, LED_R_G_FLASH_BIT);
        return;
    }
}

/******************************************************************************
 * @Function: led_R_G_turn_off
 * @Description: 红灯、绿灯关闭
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_R_G_turn_off(void) {
    LED_R_TURN_OFF();
    LED_G_TURN_OFF();
}

/******************************************************************************
 * @Function: led_server
 * @Description: LED服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void led_server(void) {
    switch (led_var.current_status) {
        case LED_STATUS_G_TURN_ON:
            led_G_turn_on();
            break;

        case LED_STATUS_R_TURN_ON:
            led_R_turn_on();
            break;

        case LED_STATUS_G_FLASH:
            led_G_flash();
            break;

        case LED_STATUS_R_FLASH:
            led_R_flash();
            break;

        case LED_STATUS_R_G_FLASH:
            led_R_G_flash();
            break;

        case LED_STATUS_R_G_TURN_OFF:
            led_R_G_turn_off();
            break;

        default:
            led_G_turn_on();
            break;
    }
}

/******************************************************************************
 * @Function: task_led_init
 * @Description: LED任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_led_init(void) {
    led_init();

    return 0;
}

/******************************************************************************
 * @Function: task_led_deInit
 * @Description: LED任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_led_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_led_run
 * @Description: LED任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_led_run(void *pvParameters) {
    task_led_init();

    while (1) {
        led_server();

        osDelay(1000);
    }
}
