/******************************************************************************
 * Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_charge.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-03-25 09:37:54
 * @Description: 充电任务文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_charge.h"
#include "fal_charge_auto.h"
#include "fal_charge_wire.h"
#include "cmsis_os.h"
#include "log.h"
#include <string.h>

charge_flag_bit_enum_t charge_var;

/******************************************************************************
 * @Function: charge_init
 * @Description: 充电初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void charge_init(void) {
    memset(&charge_var, 0, sizeof(charge_flag_bit_enum_t));
}

/******************************************************************************
 * @Function: task_charge_init
 * @Description: 充电任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_charge_init(void) {
    charge_init();
    fal_charge_auto_charge_init();
    fal_charge_wire_charge_init();

    return 0;
}

/******************************************************************************
 * @Function: task_charge_deInit
 * @Description: 充电任务反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_charge_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_charge_run
 * @Description: 充电任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_charge_run(void *pvParameters) {
    task_charge_init();
    osDelay(1000);

    while (1) {
        fal_charge_auto_charge_server();
        fal_charge_wire_charge_server();
        uros_timeout_server();
        osDelay(5);
    }
}
