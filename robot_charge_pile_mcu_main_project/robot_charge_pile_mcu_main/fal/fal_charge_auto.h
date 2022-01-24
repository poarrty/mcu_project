/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_charge_auto.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-13 20:35:09
 * @Description: 自动充电相关定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_CHARGE_AUTO_H__
#define __FAL_CHARGE_AUTO_H__

#include <stdint.h>

//自动充电超时时间：35s
#ifndef CHARGE_AUTO_TIMEOUT_CNT
#define CHARGE_AUTO_TIMEOUT_CNT 7000
#endif

#define ELECTRODE_DOWN_SEND_TIMES 5

typedef enum {
    POWER_SUPPLY_STATUS_UNKNOWN = 0,
    POWER_SUPPLY_STATUS_CHARGING = 1,
    POWER_SUPPLY_STATUS_DISCHARGING = 2,
    POWER_SUPPLY_STATUS_NOT_CHARGING = 3,
    POWER_SUPPLY_STATUS_FULL = 4,
} power_supply_status_enum_t;

typedef struct {
    uint8_t flag;
    uint16_t timeout_cnt;
    uint16_t electrode_down_send_times;
} charge_auto_charge_var_stu_t;

typedef enum {
    //标记电极是否压下
    CHARGE_AUTO_ELECTRODE_DOWN_BIT = 0,
    //发送电极压下使能标志位
    PUBLISH_ELECTRODE_DOWN_BIT,
    //发送电极弹开使能标志位
    PUBLISH_ELECTRODE_UP_BIT,
    //电极压下到充上电的超时标志位
    CHARGE_AUTO_TIMEOUT_EN_BIT,
    //污水过渡箱水位满标志位
    CL_WATER_LEVEL_FULL_BIT,
    //获取电池电量请求
    BATTERY_UPDATE_REQ_BIT,
} charge_auto_charge_flag_bit_enum_t;

typedef struct {
    //充电相关标志位
    uint8_t flag;
} charge_var_stu_t;

typedef enum {
    //停止充电使能
    CHARGE_STOP_CHARGE_EN_BIT = 0,
} charge_flag_bit_enum_t;

extern charge_flag_bit_enum_t charge_var;

extern charge_auto_charge_var_stu_t auto_charge_var;

void fal_charge_auto_charge_init(void);
void fal_charge_auto_charge_server(void);
void uros_timeout_server(void);

#endif
