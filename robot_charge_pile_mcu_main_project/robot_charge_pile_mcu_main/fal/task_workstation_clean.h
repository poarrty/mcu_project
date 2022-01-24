/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name:
 * @Author: (linzhongxing@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description:
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_WORKSTATION_CLEAN_H__
#define __TASK_WORKSTATION_CLEAN_H__

#include <stdint.h>

typedef enum {
    //开始加水
    IR_RECV_ADD_WATER_START_CMD = 0x01,
    //停止加水
    IR_RECV_ADD_WATER_STOP_CMD = 0x02,
    //开始排水
    IR_RECV_SEWAGE_START_CMD = 0x03,
    //停止排水
    IR_RECV_SEWAGE_STOP_CMD = 0x04,
    //开始清洗
    IR_RECV_TRANSITION_BOX_CLEAN_START_CMD = 0x05,
    //停止清洗
    IR_RECV_TRANSITION_BOX_CLEAN_STOP_CMD = 0x06,

} clean_cmd_enum_t;

#endif
