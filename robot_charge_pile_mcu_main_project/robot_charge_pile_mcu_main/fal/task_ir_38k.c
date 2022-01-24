/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_ir_38k.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-08-05 10:08:41
 * @Description: 38K红外任务源文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "task_ir_38k.h"
#include "pal_ir_38k.h"
#include "bsp_simuart_tx.h"
#include "fal_charge_auto.h"
#include "common_def.h"
#include "cmsis_os.h"
#include "log.h"
#include <string.h>
#include <stdio.h>

ir_38k_var_stu_t ir_38k_var;
extern uint8_t micro_connect_flag;

uint8_t ir_id = 0xA0;
extern uint8_t pal_ir_38k_make_send_msg(uint8_t id, ir_38k_send_cmd_enum_t cmd,
                                        uint8_t *msg);

uint8_t cal_checksum(uint8_t *data, uint16_t len) {
    uint8_t checksum = 0;
    uint16_t i;

    for (i = 0; i < len; i++) { checksum += data[i]; }

    return checksum;
}

/******************************************************************************
 * @Function: ir_38k_send
 * @Description: 38K红外发送消息函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {ir_38k_msg_type_enum_t} type
 * @param {ir_38k_send_cmd_enum_t} cmd
 *******************************************************************************/
void ir_38k_send(ir_38k_msg_type_enum_t type, ir_38k_send_cmd_enum_t cmd) {
    switch (type) {
        case MSG_TYPE_LOC:
            pal_ir_38k_send(ir_id, IR_38K_CH2, SEND_LOC_RIGHT_CMD);
            pal_ir_38k_send(ir_id, IR_38K_CH1, SEND_LOC_LEFT_CMD);
            break;

        case MSG_TYPE_CMD:
            pal_ir_38k_send(ir_id, IR_38K_CH1, cmd);
            pal_ir_38k_send(ir_id, IR_38K_CH2, cmd);
            break;

        default:
            LOG_WARNING("Unsupport msg type!");
            break;
    }
}

/******************************************************************************
 * @Function: ir_38k_server
 * @Description: 38K红外服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void ir_38k_server(void) {
    static uint8_t send_ble_mac_count = 0;
    static uint8_t send_down_count = 0;
    static uint8_t send_up_count = 0;

    if (SYS_GET_BIT(ir_38k_var.flag, SEND_ELECTRODE_DOWN_BIT)) {
        SYS_CLR_BIT(ir_38k_var.flag, SEND_ELECTRODE_DOWN_BIT);
        send_up_count = 0;
        send_ble_mac_count = 20;
        send_down_count = 10;
    } else if (SYS_GET_BIT(ir_38k_var.flag, SEND_ELECTRODE_UP_BIT)) {
        SYS_CLR_BIT(ir_38k_var.flag, SEND_ELECTRODE_UP_BIT);
        send_up_count = 10;
        send_ble_mac_count = 0;
        send_down_count = 0;
    }

    if (send_down_count) {
        --send_down_count;
        ir_38k_send(MSG_TYPE_CMD, SEND_ELECTRODE_DOWN_CMD);
        osDelay(40);
    } else if (send_up_count) {
        --send_up_count;
        ir_38k_send(MSG_TYPE_CMD, SEND_ELECTRODE_UP_CMD);
        osDelay(40);
    } else if (send_ble_mac_count) {
        --send_ble_mac_count;
        ir_38k_send(MSG_TYPE_CMD, SEND_BLE_MAC_CMD);
        LOG_DEBUG("send_ble_mac_count");
        osDelay(200);
    } else if (!micro_connect_flag) {
        ir_38k_send(MSG_TYPE_LOC, SEND_CMD_NONE);
        osDelay(30);
    }

    if (SYS_GET_BIT(ir_38k_var.flag, SEND_LOC_SEND_EN_ACK_BIT)) {
        SYS_CLR_BIT(ir_38k_var.flag, SEND_LOC_SEND_EN_ACK_BIT);
        ir_38k_send(MSG_TYPE_CMD, SEND_LOC_SEND_EN_ACK_CMD);
        osDelay(40);
    }
}

/******************************************************************************
 * @Function: task_ir_38k_init
 * @Description: 38K红外初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_ir_38k_init(void) {
    ir_38k_init();

    return 0;
}

/******************************************************************************
 * @Function: task_ir_38k_deInit
 * @Description: 38K红外反初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_ir_38k_deInit(void) {
    return 0;
}

/******************************************************************************
 * @Function: task_ir_38k_run
 * @Description: 38K红外任务入口文件
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_ir_38k_run(void *pvParameters) {
    task_ir_38k_init();

    while (1) {
        ir_38k_server();

        // if (i++ > 10)
        // {
        // 	LOG_INFO("%s:[%x][%x][0]\r\n", __FUNCTION__,
        // ir_rx_handle[0].ir_buff[0], ir_rx_handle[0].ir_buff[1]);
        // 	LOG_INFO("%s:[%x][%x][1]\r\n", __FUNCTION__,
        // ir_rx_handle[1].ir_buff[0], ir_rx_handle[1].ir_buff[1]); 	i = 0;
        // i = 0;
        // }
    }
}
