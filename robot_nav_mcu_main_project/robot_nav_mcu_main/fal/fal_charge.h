/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-12
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

#ifndef _FAL_CHARGE_H
#define _FAL_CHARGE_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "stdint.h"

/**
 * @ingroup FAL_CHARGE
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
typedef enum { AUTO_CHARGE_START = 1 } CHARGE_CTRL_E;

typedef enum {
    AUTO_CHARGE_STA_IDLE    = 1,
    AUTO_CHARGE_STA_CHARING = 2,
    AUTO_CHARGE_STA_FAILED  = 3,

} AUTO_CHARGE_STA_E;

typedef enum {
    CHARGE_STA_IDLE          = 1,
    CHARGE_STA_AUTO_CHARGING = 2,
    CHARGE_STA_PUSH_CHARGING = 3,
    CHARGE_STA_WIRE_CHARGING = 4

} CHARGE_STA_E;

typedef enum {
    SEND_LOC_RIGHT_CMD       = 0x5F,
    SEND_LOC_LEFT_CMD        = 0xF6,
    SEND_ELECTRODE_DOWN_CMD  = 0x04,
    SEND_LOC_SEND_EN_ACK_CMD = 0x05,
    SEND_ELECTRODE_UP_CMD    = 0x06,
    SEND_BLE_MAC_CMD         = 0xCC,
} ir_38k_send_cmd_enum_t;

typedef enum {
    CHARGE_ELEC_UP         = 0,
    AUTO_CHARGE_ELEC_PRESS = 0x01,
    PUSH_CHARGE_ELEC_PRESS = 0x02,
} CHARGE_FEEDBACK_E;

typedef enum {
    CHARGE_CABLE_STATE_CANCEL = 0x00,
    CHARGE_CABLE_STATE_INSERT = 0x01,
} CHARGE_CABLE_STATE;

#define CABLE_CHARGE_ALL_BIT_HIGH 0xFF
#define CABLE_CHARGE_ALL_BIT_LOW  0x00

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/****************************************************
功能  : 初始化 fal_charge
返回值: 等于0表示成功，其它值表示失败原因
 ***************************************************/
int fal_charge_init(void);

/****************************************************
功能  ：释放 fal_charge 层资源
返回值：等于0表示成功，其它值表示失败原因
***************************************************/
int fal_charge_deInit(void);

void    fal_charge_relay_on(void);
void    fal_charge_relay_off(void);
uint8_t fal_charge_get_packing_req(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} FAL_CHARGE */

#endif
