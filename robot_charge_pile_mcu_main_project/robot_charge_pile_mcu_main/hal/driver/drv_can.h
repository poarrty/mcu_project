/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_can.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 15:49:32
 * @Description: CAN驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_CAN_H__
#define __DRV_CAN_H__

#include "can.h"
#include <stdint.h>

typedef struct {
    uint32_t id;
    uint8_t type;
    uint8_t data_len;
    uint8_t *data;
} can_msg_stu_t;

HAL_StatusTypeDef drv_can_init(void);
HAL_StatusTypeDef drv_can_send_msg(can_msg_stu_t can_tx_msg);

#endif
