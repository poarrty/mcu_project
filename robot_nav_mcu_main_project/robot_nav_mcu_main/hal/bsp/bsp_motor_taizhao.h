/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         linzhongxing
 ** Version:        V0.0.1
 ** Date:           2021-9-18
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-09 linzhongxing 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-9-18       linzhongxing    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_MOTOR_TAIZHAO_H
#define _BSP_MOTOR_TAIZHAO_H

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
 * @ingroup BSP_MOTOR_DONGXING
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
#define MOTOR_CAN_H hcan1
#define MOTOR_CAN_I CAN1

// COBID
#define COBID_SYNC 0x80

#define COBID_SPEED_L 0x301
#define COBID_SPEED_R 0x302

#define COBID_18L 0x181
#define COBID_28L 0x281
#define COBID_38L 0x381
#define COBID_48L 0x481

#define COBID_18R 0x182
#define COBID_28R 0x282
#define COBID_38R 0x382
#define COBID_48R 0x482

#define COBID_SDO_RESP_L 0x581
#define COBID_SDO_RESP_R 0x582

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef struct {
    uint16_t status_world;
    uint16_t error_code;
    uint32_t exception_code;

} MOTOR_TPDO1_T;

typedef struct {
    uint32_t speed;
    uint32_t position;

} MOTOR_TPDO2_T;

typedef struct {
    uint16_t temp;
    uint16_t current;
    uint16_t voltage;
    uint16_t opt_mode;

} MOTOR_TPDO3_T;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

int     bsp_motor_taizhao_init(void);
uint8_t bsp_taizhao_motor_disable(void);
uint8_t bsp_taizhao_motor_enable(void);
uint8_t bsp_motor_taizhao_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock);
void    taizhao_sdo_resp_msg(uint8_t *buff);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR_DONGXING */

#endif
