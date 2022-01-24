/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_ir_38k.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-07-23 16:06:26
 * @Description: 38K红外通信接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_usart.h"
#include "pal_ir_38k.h"
#include "task_ir_38k.h"
#include "bsp_simuart_tx.h"
#include "fal_charge_auto.h"
#include "tim.h"
#include "crc16.h"
#include "log.h"
#include <stdio.h>

SimUart_Tx_TypeDef simuart_tx1;
SimUart_Tx_TypeDef simuart_tx2;

extern ir_38k_var_stu_t ir_38k_var;

/******************************************************************************
 * @Function: ir_38k_init
 * @Description: 38K红外初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void ir_38k_init(void) {
    memset(&ir_38k_var, 0, sizeof(ir_38k_var_stu_t));

    simuart_tx1.baudrate = 1200;
    // simuart_tx1.gpio_port = SIM_UART_TX1_GPIO_Port;
    // simuart_tx1.gpio_pin = SIM_UART_TX1_Pin;
    simuart_tx1.tim = htim6;
    simuart_tx1.frq_div_clock = 1000000;
    simuart_tx1.tx_tim = htim2;
    simuart_tx1.tx_channel = TIM_CHANNEL_1;

    simuart_tx2.baudrate = 1200;
    // simuart_tx2.gpio_port = SIM_UART_TX2_GPIO_Port;
    // simuart_tx2.gpio_pin = SIM_UART_TX2_Pin;
    simuart_tx2.tim = htim7;
    simuart_tx2.frq_div_clock = 1000000;
    simuart_tx2.tx_tim = htim2;
    simuart_tx2.tx_channel = TIM_CHANNEL_2;

    bsp_simuart_tx_init(&simuart_tx1);
    bsp_simuart_tx_init(&simuart_tx2);
}

/******************************************************************************
 * @Function: get_pile_id
 * @Description: 获取桩ID
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint16_t get_pile_id(void) {
    return 0;
}

/******************************************************************************
 * @Function: pal_ir_38k_make_send_msg
 * @Description: 发送数据组帧
 * @Input: cmd：要发送的指令
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {ir_38k_send_cmd_enum_t} cmd
 * @param {uint8_t} *msg
 *******************************************************************************/
uint8_t pal_ir_38k_make_send_msg(uint8_t id, ir_38k_send_cmd_enum_t cmd,
                                 uint8_t *msg) {
    uint8_t len = 0;
    extern uint8_t blue_address[6];

    switch (cmd) {
        case SEND_ELECTRODE_DOWN_CMD:
        case SEND_ELECTRODE_UP_CMD:
            msg[0] = id;
            msg[1] = cmd;
            msg[2] = ~cmd;
            len = 3;
            break;

        case SEND_BLE_MAC_CMD:
            msg[0] = id;
            msg[1] = SEND_BLE_MAC_CMD;
            memcpy(&msg[2], blue_address, 6);
            msg[8] = 0;

            for (int i = 0; i < 8; i++) { msg[8] += msg[i]; }

            len = 9;
            break;

        default:
            msg[0] = id;
            msg[1] = cmd;
            len = 2;
            break;
    }

    return len;
}

/******************************************************************************
 * @Function: pal_ir_38k_send
 * @Description: 38K红外发送函数
 * @Input: ch_index：通道索引
 *         f_index：数据帧索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {ir_38k_channel_enum_t} channel
 * @param {ir_38k_send_cmd_enum_t} cmd
 *******************************************************************************/
void pal_ir_38k_send(uint8_t id, ir_38k_channel_enum_t channel,
                     ir_38k_send_cmd_enum_t cmd) {
    uint8_t send_msg[USART_BUFF_MAX_SIZE] = {0};
    uint8_t send_len = 0;

    send_len = pal_ir_38k_make_send_msg(id, cmd, send_msg);

    switch (channel) {
        case IR_38K_CH1:
            bsp_simuart_tx_send_buff(&simuart_tx1, send_msg, send_len);
            break;

        case IR_38K_CH2:
            bsp_simuart_tx_send_buff(&simuart_tx2, send_msg, send_len);
            break;

        default:
            LOG_WARNING("Unsupport usart index!");
            break;
    }
}

/******************************************************************************
 * @Function: pal_ir_38k_recv_cmd_callbake
 * @Description: 38K红外接收指令回调函数
 * @Input: buff：传入的buff
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {ir_38k_recv_cmd_enum_t} r_cmd
 *******************************************************************************/
void pal_ir_38k_recv_cmd_callbake(uint8_t *buff) {
    switch (buff[0]) {
        case RECV_LOC_SEND_EN_CMD:
            // ir_38k_var.sn = usart3.rx_buff[3];
            // SYS_SET_BIT(ir_38k_var.flag, SEND_LOC_SEND_EN_ACK_BIT);
            break;

        case RECV_LOC_SEND_ACK_CMD:
            // ir_38k_var.num = ((ir_38k_var.sn == usart3.rx_buff[3]) ? 1000 :
            // ir_38k_var.num);
            break;

        default:
            LOG_WARNING("Unsupport cmd!");
            break;
    }
}
