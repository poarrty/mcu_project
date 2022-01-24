/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_usart3.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-30 15:16:45
 * @Description: USART3的协议层头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __PAL_USART3_H__
#define __PAL_USART3_H__

#include "drv_usart.h"
#include "bsp_s09.h"
#include <stdint.h>

typedef enum {
    PAL_USART3_CMD_TYPE_POWER_ON = 1,
    PAL_USART3_CMD_TYPE_POWER_OFF = 2,
    PAL_USART3_CMD_TYPE_UITRASONIC = 3,
    PAL_USART3_CMD_TYPE_UITRASONIC_EN = 4,
    PAL_USART3_CMD_TYPE_UITRASONIC_SWITCH = 5,
} pal_uasrt3_frame_cmd_type_enum_t;

typedef enum {
    PAL_USART3_POWER_MANAGER_RESPOND_TYPE_NORMAL = 1,
    PAL_USART3_POWER_MANAGER_RESPOND_TYPE_TIMEOUT = 2,
} pal_usart3_power_manager_respond_type_enum_t;

#pragma pack(push)
#pragma pack(1)

//电源管理帧格式
typedef struct {
    uint8_t head;
    uint8_t len;
    uint8_t cmd;
    uint8_t respond_flag;
    uint8_t checksum;
} pal_usart3_send_power_manager_frame_stu_t;

//超声数据帧格式
typedef struct {
    uint8_t head;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[UITRASONIC_CHANNEL_NUM * 2];
    uint8_t checksum;
} pal_usart3_send_uitrasonic_frame_stu_t;

#pragma pack(pop)

void pal_usart3_recv_msg_parse(usart_info_stu_t usart);
void pal_usart3_send_power_manager_respond_msg(
    pal_uasrt3_frame_cmd_type_enum_t cmd,
    pal_usart3_power_manager_respond_type_enum_t respond_flag);
void pal_usart3_send_uitrasonic_distance_msg(uint8_t *data, uint16_t data_len);

#endif
