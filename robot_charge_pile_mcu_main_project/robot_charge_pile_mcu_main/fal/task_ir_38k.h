/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_ir_38k.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-08-05 10:08:16
 * @Description: 38K红外任务头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_IR_38K_H__
#define __TASK_IR_38K_H__

#include <stdint.h>

typedef enum {
    SEND_ELECTRODE_DOWN_BIT = 0,
    SEND_ELECTRODE_UP_BIT,
    SEND_LOC_SEND_EN_ACK_BIT,
} ir_38k_flag_bit_enum_t;

typedef enum {
    MSG_TYPE_LOC = 0,
    MSG_TYPE_CMD = 1,
    MSG_TYPE_A0 = 2,
    MSG_TYPE_NONE = 3,
} ir_38k_msg_type_enum_t;

typedef enum {
    IR_38K_1T1R = 1,
    IR_38K_3T2R = 3,
} ir_38k_send_mode_enum_t;

typedef struct {
    // 38K IR相关标志位
    uint8_t flag;
    // 38K IR连续发送次数
    uint16_t send_times;
    // 38K IR通信序号
    uint8_t sn;
    ir_38k_msg_type_enum_t msg_type;
    uint8_t msg;
    uint16_t ir_timeout_cnt;
} ir_38k_var_stu_t;

extern ir_38k_var_stu_t ir_38k_var;

void ir_38k_init(void);
void ir_38k_server(void);
int task_ir_38k_init(void);
int task_ir_38k_deInit(void);

#endif
