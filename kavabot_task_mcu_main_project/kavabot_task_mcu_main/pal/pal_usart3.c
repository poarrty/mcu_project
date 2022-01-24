/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_usart3.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-30 15:17:05
 * @Description: USART3的协议层文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "pal_usart3.h"
#include "common_def.h"
#include "fal_power.h"
#include "bsp_s09.h"
#include "log.h"
#include <string.h>

/******************************************************************************
 * @Function: calculate_buff_checksum
 * @Description: 计算checksum
 * @Input: buff：数据地址
 *         len：数据长度
 * @Output: None
 * @Return: 返回计算结果
 * @Others: None
 * @param {uint8_t} buff
 * @param {uint16_t} len
 *******************************************************************************/
uint16_t calculate_buff_checksum(uint8_t *buff, uint16_t len) {
    uint16_t checksum = 0;

    while (len--) { checksum += buff[len]; }

    return checksum;
}

/******************************************************************************
 * @Function: pal_usart3_recv_power_on_cmd_callbake
 * @Description: USART3接收到开机指令的回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void pal_usart3_recv_power_on_cmd_callbake(void) {
    if (!SYS_GET_BIT(power_var.flag, POWER_CURRENT_STATUS_BIT)) {
        fal_sys_power_on();
        SYS_CLR_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT);
        SYS_SET_BIT(power_var.flag, POWER_ON_REQUEST_EN_BIT);
    }
}

/******************************************************************************
 * @Function: pal_usart3_recv_power_off_cmd_callbake
 * @Description: USART3接收到关机指令的回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void pal_usart3_recv_power_off_cmd_callbake(void) {
    if (SYS_GET_BIT(power_var.flag, POWER_CURRENT_STATUS_BIT)) {
        SYS_CLR_BIT(power_var.flag, POWER_ON_REQUEST_EN_BIT);
        SYS_SET_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT);
    }
}

/******************************************************************************
 * @Function: pal_usart3_recv_uitrasonic_en_cmd_callbake
 * @Description: USART3接收到超声使能指令的回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void pal_usart3_recv_uitrasonic_en_cmd_callbake(void) {
    SYS_SET_BIT(s09_var.flag, UITRASONIC_DETECT_EN_BIT);
}

/******************************************************************************
 * @Function: pal_usart3_recv_msg_parse
 * @Description: USART3接收数据解析函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {usart_info_stu_t} usart
 *******************************************************************************/
void pal_usart3_recv_msg_parse(usart_info_stu_t usart) {
    LOG_DEBUG_ARRAY(usart.rx_buff, usart.rx_len);

    if (usart.rx_buff[usart.rx_len - 1] ==
        (calculate_buff_checksum(usart.rx_buff, usart.rx_buff[1] + 2) & 0xFF)) {
        switch (usart.rx_buff[2]) {
            case PAL_USART3_CMD_TYPE_POWER_ON:
                pal_usart3_recv_power_on_cmd_callbake();
                break;

            case PAL_USART3_CMD_TYPE_POWER_OFF:
                pal_usart3_recv_power_off_cmd_callbake();
                break;

            case PAL_USART3_CMD_TYPE_UITRASONIC_EN:
                pal_usart3_recv_uitrasonic_en_cmd_callbake();
                break;

            default:
                LOG_ERROR("Not support cmd!");
                break;
        }
    } else {
        LOG_ERROR("Checksum is error!");
    }
}

/******************************************************************************
 * @Function: pal_usart3_send_power_manager_respond_msg
 * @Description: USART3发送电源管理响应信息
 * @Input: cmd：指令类型
 *         respond_flag：响应标志
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {pal_uasrt3_frame_cmd_type_enum_t} cmd
 * @param {pal_usart3_power_manager_respond_type_enum_t} respond_flag
 *******************************************************************************/
void pal_usart3_send_power_manager_respond_msg(
    pal_uasrt3_frame_cmd_type_enum_t cmd,
    pal_usart3_power_manager_respond_type_enum_t respond_flag) {
    pal_usart3_send_power_manager_frame_stu_t frame;
    uint16_t checksum = 0;

    frame.head = 0x2E;
    frame.len = 2;
    frame.cmd = cmd;
    frame.respond_flag = respond_flag;

    checksum += frame.head;
    checksum += frame.len;
    checksum += frame.cmd;
    checksum += frame.respond_flag;
    frame.checksum = checksum & 0xFF;

    LOG_DEBUG_ARRAY((uint8_t *) &frame,
                    sizeof(pal_usart3_send_power_manager_frame_stu_t));

    drv_usart3_send_buffer((uint8_t *) &frame,
                           sizeof(pal_usart3_send_power_manager_frame_stu_t));
}

/******************************************************************************
 * @Function: pal_usart3_send_uitrasonic_distance_msg
 * @Description: USART3发送超声距离信息函数
 * @Input: data：超声数据地址
 *         data_len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *data
 * @param {uint16_t} data_len
 *******************************************************************************/
void pal_usart3_send_uitrasonic_distance_msg(uint8_t *data, uint16_t data_len) {
    pal_usart3_send_uitrasonic_frame_stu_t frame;
    uint16_t checksum = 0;

    frame.head = 0x2E;
    frame.len = data_len + 1;
    frame.cmd = PAL_USART3_CMD_TYPE_UITRASONIC;
    memcpy(frame.data, data, data_len);

    checksum = calculate_buff_checksum(data, data_len);
    checksum += frame.head;
    checksum += frame.len;
    checksum += frame.cmd;
    frame.checksum = checksum & 0xFF;

    LOG_DEBUG_ARRAY((uint8_t *) &frame,
                    sizeof(pal_usart3_send_uitrasonic_frame_stu_t));

    drv_usart3_send_buffer((uint8_t *) &frame,
                           sizeof(pal_usart3_send_uitrasonic_frame_stu_t));
}
