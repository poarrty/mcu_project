/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         linzhongxing
 ** Version:        V0.0.1
 ** Date:           2021-8-2
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-08 linzhongxing 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-8-2       linzhongxing    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_MOTOR_FENGDEKONG_H
#define _BSP_MOTOR_FENGDEKONG_H

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
 * @ingroup BSP_MOTOR_fengdekong
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
#define SYNTRON_CAN_MOTOR_H         hcan1
#define SYNTRON_CAN_MOTOR_SYNTRON_I CAN1

#define Message_SDO_NODE1_RX1 0x581
#define Message_SDO_NODE2_RX1 0x582
#define SYNTRON_CAN_ID_L      Message_SDO_NODE2_RX1
#define SYNTRON_CAN_ID_R      Message_SDO_NODE1_RX1

#define SYNTRON_CAN_ID_L_POS 0X182
#define SYNTRON_CAN_ID_R_POS 0X181

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_motor_fengdekong_init
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_motor_fengdekong_init(void);

/*****************************************************************
功  能: 释放 bsp_motor_fengdekong_deInit 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_motor_fengdekong_deInit(void);

uint8_t bsp_motor_fengdekong_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock);

uint8_t bsp_motor_fengdekong_req_pos(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR_fengdekong */

#endif
