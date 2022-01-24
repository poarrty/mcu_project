/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_button.c
 * @Author: LinZhongXing(linzhongxing@cvte.com)
 * @Version: V1.0
 * @Date: 2021-08-4 10:05:00
 * @Description: 测试模式任务入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_test_mode.h"

#include "log.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdint.h>
#include "task_digital_tube.h"
#include "bsp_clean_fun.h"

test_mode_enum_t test_mode_status = TEST_OFF;
uint8_t test_led_cnt = 0;

/******************************************************************************
 * @Function: test_mode_init
 * @Description: 初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void test_mode_init(void) {}

/******************************************************************************
 * @Function: task_test_mode_init
 * @Description: 任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_test_mode_init(void) {
    test_mode_init();

    return 0;
}

void set_test_mode(void) {
    test_mode_status = TEST_ON;
    all_device_start();
}

/******************************************************************************
 * @Function: task_test_mode_run
 * @Description: 测试模式控制任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_test_mode_run(void *argument) {
    task_test_mode_init();

    while (1) {
        if (test_mode_status == TEST_ON) {
            digital_tube_display_on(test_led_cnt++, DYNAMIC_DISPLAY,
                                    BATTERY_DISPLAY);
            test_led_cnt %= 100;
        }

        // LOG_DEBUG("power_on:[%u]", power_on_cnt++);
        osDelay(1000);
    }
}
