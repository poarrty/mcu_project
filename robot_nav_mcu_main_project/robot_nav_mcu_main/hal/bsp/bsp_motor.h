/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-26
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_MOTOR_H
#define _BSP_MOTOR_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "stdint.h"
#include "can.h"
#include "common.h"

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup BSP_MOTOR
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define CAN_MOTOR_H hcan1
#define CAN_MOTOR_I CAN1

#define CAN_ID_SET_DPEED 0X00801400
#define CAN_ID_TEST      0X00800C00

#define CAN_ID_TEST_RESP 0X00020C00
#define CAN_ID_ERROR_STA 0X00021400
#define CAN_ID_POSITION  0X00024C00

#define CAN_ID_UPDATE_SELF  (0X03000000)
#define CAN_ID_UPDATE_MOTOR (0X000C0000)

#define CAN_CMD_UPDATE_START      (0X08)
#define CAN_CMD_UPDATE_HANDSHAKE  (0X00)
#define CAN_CMD_UPDATE_HEAD_ACK   (0X01)
#define CAN_CMD_UPDATE_PACK_REQ   (0x02)
#define CAN_CMD_UPDATE_FINISH_REQ (0X03)
#define CAN_CMD_UPDATE_FAILED     (0X04)
#define CAN_CMD_UPDATE_SUCCESS    (0X05)

#define CAN_ID_UPDATE_START (0X804400)

#define CAN_SEND_BITPOS (23)
#define CAN_RECV_BITPOS (17)
#define CAN_CMD_BITPOS  (11)
#define CAN_END_BITPOS  (10)

#define CAN_ID_UPDATE_HANDSHAKE_REQ (CAN_ID_UPDATE_MOTOR | (CAN_CMD_UPDATE_HANDSHAKE << CAN_CMD_BITPOS))
#define CAN_ID_UPDATE_HANDSHAKE_ACK (CAN_ID_UPDATE_SELF | (CAN_CMD_UPDATE_HANDSHAKE << CAN_CMD_BITPOS))

#define CAN_ID_UPDATE_PACK_REQ (CAN_ID_UPDATE_MOTOR | (CAN_CMD_UPDATE_PACK_REQ << CAN_CMD_BITPOS))
#define CAN_ID_UPDATE_HEAD_ACK (CAN_ID_UPDATE_SELF | (CAN_CMD_UPDATE_HEAD_ACK << CAN_CMD_BITPOS))
#define CAN_ID_UPDATE_PACK_ACK (CAN_ID_UPDATE_SELF | (CAN_CMD_UPDATE_PACK_REQ << CAN_CMD_BITPOS))

#define CAN_ID_UPDATE_FINISH_REQ (CAN_ID_UPDATE_MOTOR | (CAN_CMD_UPDATE_FINISH_REQ << CAN_CMD_BITPOS))
#define CAN_ID_UPDATE_FINISH_ACK (CAN_ID_UPDATE_SELF | (CAN_CMD_UPDATE_FINISH_REQ << CAN_CMD_BITPOS))

#define CAN_ID_UPDATE_FAILED  (CAN_ID_UPDATE_MOTOR | (CAN_CMD_UPDATE_FAILED << CAN_CMD_BITPOS))
#define CAN_ID_UPDATE_SUCCESS (CAN_ID_UPDATE_MOTOR | (CAN_CMD_UPDATE_SUCCESS << CAN_CMD_BITPOS))

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef struct {
    unsigned short int : 1;         ///< 右轴电机过流
    unsigned short int : 1;         ///< 左轴电机过流
    unsigned short int : 1;         ///< 左轴电机霍尔故障
    unsigned short int : 1;         ///< 右轴电机霍尔故障
    unsigned short int lock_l : 1;  ///< 左轴电机刹车故障
    unsigned short int lock_r : 1;  ///< 右轴电机刹车故障
    unsigned short int : 1;         ///< 主电路预充故障
    unsigned short int : 1;         ///< 主电路过压，继电器不吸合
    unsigned short int : 1;         ///< 右电机堵转
    unsigned short int : 1;         ///< 左电机堵转
    unsigned short int : 1;         ///< 右电机MOS上桥异常
    unsigned short int : 1;         ///< 左电机MOS上桥异常
    unsigned short int : 1;         ///< 右电机MOS下桥异常
    unsigned short int : 1;         ///< 左电机MOS下桥异常
    unsigned short int : 1;         ///< 充电器充电检测
    unsigned short int : 1;         ///< MCU过热

} MOTOR_ERROR_BITS_T, *MOTOR_ERROR_BITS_P;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_motor
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_motor_init(void);

/*****************************************************************
功  能: 释放 bsp_motor 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_motor_deInit(void);

uint8_t bsp_motor_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock);
uint8_t bsp_motor_send_frame(uint32_t frame_id, uint8_t *frame_data, uint8_t len);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR */

#endif
