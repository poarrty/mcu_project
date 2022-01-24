/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_usart4.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-28 14:22:47
 * @Description: USART4的协议层文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "pal_usart4.h"
#include "drv_usart.h"
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
 * @Function: pal_usart4_send_uitrasonic_distance_msg
 * @Description: USART4发送超声距离信息函数
 * @Input: data：超声数据地址
 *         data_len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *data
 * @param {uint16_t} data_len
 *******************************************************************************/
void pal_usart4_send_uitrasonic_distance_msg(uint8_t *data, uint16_t data_len) {
    pal_usart4_send_uitrasonic_frame_stu_t frame;
    uint16_t checksum = 0;

    frame.head = 0x55;
    frame.len = data_len + 1;
    frame.cmd = PAL_USART4_CMD_TYPE_UITRASONIC;
    memcpy(frame.data, data, data_len);

    checksum = calculate_buff_checksum(data, data_len);
    checksum += frame.head;
    checksum += frame.len;
    checksum += frame.cmd;
    frame.checksum = checksum & 0xFF;

    LOG_DEBUG_ARRAY((uint8_t *) &frame,
                    sizeof(pal_usart4_send_uitrasonic_frame_stu_t));

    drv_usart4_send_buffer((uint8_t *) &frame,
                           sizeof(pal_usart4_send_uitrasonic_frame_stu_t));
}
