/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_usart4.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-28 14:22:16
 * @Description: USART4的协议层头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __PAL_USART4_H__
#define __PAL_USART4_H__

#include <stdint.h>
#include <bsp_09.h>

typedef enum {
    PAL_USART4_CMD_TYPE_UITRASONIC = 3,
} pal_uasrt_frame_cmd_type_enum_t;

#pragma pack(push)
#pragma pack(1)

//超声数据帧格式
typedef struct {
    uint8_t head;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[ELMOS09_CHANNEL_NUM * 2];
    uint8_t checksum;
} pal_usart4_send_uitrasonic_frame_stu_t;

#pragma pack(pop)

void pal_usart4_send_uitrasonic_distance_msg(uint8_t *data, uint16_t data_len);

#endif
