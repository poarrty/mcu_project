/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_ir_38k.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-07-23 16:06:26
 * @Description: 38K红外通信接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __PAL_IR_38K_H__
#define __PAL_IR_38K_H__

#include <stdint.h>

#define IR_CMD_CHECK_MASK 0xFF           //指令校验掩码
#define IR_CMD_RESPONDS_CHECK_MASK 0xFE  //指令响应校验掩码

#define IR_MAKE_CMD(cmd) ((cmd << 8) | (IR_CMD_CHECK_MASK - cmd))
#define IR_MAKE_DYNAMIC_BATTERY_PERCENTAGE(val) ((val << 8) | (val))
#define IR_MAKE_CMD_RESPONDS(cmd) \
    ((cmd << 8) | (IR_CMD_RESPONDS_CHECK_MASK - cmd))

typedef enum {
    SEND_LOC_RIGHT_CMD = 0x5F,
    SEND_LOC_LEFT_CMD = 0xF6,
    SEND_ELECTRODE_DOWN_CMD = 0x04,
    SEND_LOC_SEND_EN_ACK_CMD = 0x05,
    SEND_ELECTRODE_UP_CMD = 0x06,
    SEND_BLE_MAC_CMD = 0xCC,
    SEND_CMD_NONE,
} ir_38k_send_cmd_enum_t;

typedef enum {
    RECV_LOC_SEND_EN_CMD = 0x55,
    RECV_LOC_SEND_ACK_CMD = 0x56,
} ir_38k_recv_cmd_enum_t;

typedef enum {
    IR_38K_CH1 = 1,
    IR_38K_CH2,
    IR_38K_CH3,
} ir_38k_channel_enum_t;

uint16_t get_pile_id(void);
void pal_ir_38k_send(uint8_t id, ir_38k_channel_enum_t channel,
                     ir_38k_send_cmd_enum_t cmd);

void pal_ir_38k_recv_cmd_callbake(uint8_t *buff);

#endif
