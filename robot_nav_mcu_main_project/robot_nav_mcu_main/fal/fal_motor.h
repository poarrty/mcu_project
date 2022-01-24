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

#ifndef _FAL_MOTOR_H
#define _FAL_MOTOR_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "stdint.h"

/**
 * @ingroup FAL_MOTOR 电机驱动控制
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

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef enum {
    MOTOR_RESET,
    MOTOR_ENABLE,
    MOTOR_DISABLE,
    MOTOR_MAX_CURRENT,

} MOTOR_CTRL_E;

typedef enum {
    MOTOR_ERROR_CODE_L,
    MOTOR_ERROR_CODE_R,
    MOTOR_CURRENT_L,
    MOTOR_CURRENT_R,
    MOTOR_ENCODER_L,
    MOTOR_ENCODER_R,
    MOTOR_SPEED_L,
    MOTOR_SPEED_R,

    MOTOR_READY = (1 << MOTOR_ERROR_CODE_L) | (1 << MOTOR_ERROR_CODE_R) | (1 << MOTOR_CURRENT_L) | (1 << MOTOR_CURRENT_R) |
                  (1 << MOTOR_ENCODER_L) | (1 << MOTOR_ENCODER_R) | (1 << MOTOR_SPEED_L) | (1 << MOTOR_SPEED_R),

} MOTOR_STATUS_E;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/****************************************************
功能  : 初始化 fal_motor
返回值: 等于0表示成功，其它值表示失败原因
 ***************************************************/
int fal_motor_init(void);

/****************************************************
功能  ：释放 fal_motor 资源
返回值：等于0表示成功，其它值表示失败原因
***************************************************/
int fal_motor_deInit(void);

uint8_t motor_speed_vw_set(int16_t speed_v, int16_t speed_w);
uint8_t motor_speed_vw_get(int16_t *speed_v, int16_t *speed_w);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} FAL_MOTOR */

#endif
