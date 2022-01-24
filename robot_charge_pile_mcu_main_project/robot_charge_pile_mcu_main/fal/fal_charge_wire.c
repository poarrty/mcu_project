/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_charge_wire.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-22 10:40:33
 * @Description: 有线充电文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_charge.h"
#include "fal_charge_wire.h"
#include "task_battery.h"
#include "task_digital_tube.h"
#include "drv_gpio.h"
#include "bsp_digital_tube.h"
#include "common_def.h"
#include <string.h>

charge_wire_charge_var_stu_t wire_charge_var;

/******************************************************************************
 * @Function: charge_wire_charge_init
 * @Description: 有线充电初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_charge_wire_charge_init(void) {
    memset(&wire_charge_var, 0, sizeof(charge_wire_charge_var_stu_t));
}

/******************************************************************************
 * @Function: charge_wire_charge_server
 * @Description: 有线充电服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_charge_wire_charge_server(void) {
    //有线充电触发
    if (SYS_GET_BIT(wire_charge_var.flag, CHARGE_WIRE_KEY_TRIGGER_BIT) ||
        (SYS_GET_BIT(wire_charge_var.flag, CHARGE_WIRE_COM_TRIGGER_BIT))) {
        //充电中
        if (get_battery_charge_stutus_bit()) {
            digital_tube_display_on(get_battery_percentage(), DYNAMIC_DISPLAY,
                                    BATTERY_DISPLAY);
        } else {
            digital_tube_display_on(get_battery_percentage(), STATIC_DISPLAY,
                                    BATTERY_DISPLAY);
        }
    }
}
