/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-19
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_BATTERY_H
#define _BSP_BATTERY_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "stdint.h"

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup BSP_BATTERY
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
#define CAN_BATTERY_H hcan2
#define CAN_BATTERY_I CAN2

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef struct {
    unsigned short int cell_v_low : 1;                    ///< 电芯低压
    unsigned short int cell_v_hight : 1;                  ///< 电芯高压
    unsigned short int total_v_low : 1;                   ///< 总压低压
    unsigned short int total_v_hight : 1;                 ///< 总压高压
    unsigned short int first_current_out_over_flow : 1;   ///< 一级放电过流
    unsigned short int second_current_out_over_flow : 1;  ///< 二级放电过流
    unsigned short int short_connect : 1;                 ///< 短路
    unsigned short int first_current_in_over_flow : 1;    ///< 一级充电过流
    unsigned short int second_current_in_over_flow : 1;   ///< 二级充电过流
    unsigned short int current_out_temp_hight : 1;        ///< 放电高温
    unsigned short int current_out_temp_low : 1;          ///< 放电低温
    unsigned short int current_in_temp_hight : 1;         ///< 充电高温
    unsigned short int current_in_temp_low : 1;           ///< 充电低温
    unsigned short int mos_temp_hight : 1;                ///< MOS 高温
    unsigned short int : 1;                               ///<
    unsigned short int : 1;                               ///<
} STA_PROTECT_BITS;

typedef struct {
    unsigned short int cell_v_low : 1;              ///< 电芯低压
    unsigned short int cell_v_hight : 1;            ///< 电芯高压
    unsigned short int total_v_low : 1;             ///< 总压低压
    unsigned short int total_v_hight : 1;           ///< 总压高压
    unsigned short int current_out_over_flow : 1;   ///< 放电过流
    unsigned short int current_in_over_flow : 1;    ///< 充电过流
    unsigned short int current_sample : 1;          ///< 电流采集
    unsigned short int current_out_temp_hight : 1;  ///< 放电高温
    unsigned short int current_out_temp_low : 1;    ///< 放电低温
    unsigned short int current_in_temp_hight : 1;   ///< 充电高温
    unsigned short int current_in_temp_low : 1;     ///< 充电低温
    unsigned short int mos_temp_hight : 1;          ///< MOS 高温
    unsigned short int cap_low : 1;                 ///< 低容量
    unsigned short int afe_sample : 1;              ///< AFE 采集
    unsigned short int : 1;                         ///<
    unsigned short int : 1;                         ///<
} STA_WARMING_BITS;

typedef struct {
    unsigned short int discharge_tube : 1;  ///< 放电管
    unsigned short int charge_tube : 1;     ///< 充电管
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int discharge_sta : 1;  ///< 放电状态
    unsigned short int charge_sta : 1;     ///< 充电状态
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
    unsigned short int : 1;
} STA_BMS_BITS;

typedef struct {
    unsigned short int strand1 : 1;  ///< 第1串均衡
    unsigned short int strand2 : 1;
    unsigned short int strand3 : 1;
    unsigned short int strand4 : 1;
    unsigned short int strand5 : 1;
    unsigned short int strand6 : 1;
    unsigned short int strand7 : 1;
    unsigned short int strand8 : 1;
    unsigned short int strand9 : 1;
    unsigned short int strand10 : 1;
    unsigned short int strand11 : 1;
    unsigned short int strand12 : 1;
    unsigned short int strand13 : 1;
    unsigned short int strand14 : 1;
    unsigned short int strand15 : 1;
    unsigned short int strand16 : 1;

} STA_BAL_BITS;

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t header;
    uint8_t data[7];

} BATTERY_BMS_PACK_T, *BATTERY_BMS_PACK_P;

typedef struct {
    int32_t          current;            ///< 电流
    uint32_t         cap_full_in;        ///< 满充容量
    uint32_t         cap_full_out;       ///< 满放容量
    uint32_t         cap_remain;         ///< 剩余容量
    uint16_t         voltage;            ///< 总电压
    uint16_t         rsos;               ///< RSOC
    uint16_t         cyc_time;           ///< 循环次数
    STA_PROTECT_BITS sta_protect;        ///< 保护状态字节
    STA_WARMING_BITS sta_warming;        ///< 告警状态字节
    STA_BMS_BITS     sta_bms;            ///< BMS 状态字节
    STA_BAL_BITS     sta_bla;            ///< 均衡状态字节
    int16_t          temp_cell1;         ///< 电芯温度1
    int16_t          temp_cell2;         ///< 电芯温度2
    int16_t          temp_cell3;         ///< 电芯温度3
    int16_t          temp_cell4;         ///< 电芯温度4
    int16_t          temp_env;           ///< 环境温度
    int16_t          temp_power_board1;  ///< 功率板1温度
    int16_t          temp_power_board2;  ///< 功率板2温度
    uint16_t         vol_cell11;         ///< Cell1 电压
    uint16_t         vol_cell12;         ///<
    uint16_t         vol_cell13;         ///<
    uint16_t         vol_cell14;         ///<
    uint16_t         vol_cell15;         ///<
    uint16_t         vol_cell16;         ///<
    uint16_t         vol_cell17;         ///<
    uint16_t         vol_cell18;         ///<
    uint16_t         vol_cell19;         ///<
    uint16_t         vol_cell110;        ///<
    uint16_t         vol_cell111;        ///<
    uint16_t         vol_cell112;        ///<
    uint16_t         vol_cell113;        ///<
    uint16_t         vol_cell114;        ///<
    // uint16_t vol_cell115;
    // uint16_t vol_cell116;

} BATTERY_BMS_DATA_T, *BATTERY_BMS_DATA_P;

typedef struct {
    uint8_t            addr;
    uint8_t            len;
    uint8_t            func;
    BATTERY_BMS_DATA_T bms_data;
    uint16_t           crc;

} BATTERY_BMS_FRAME_T, *BATTERY_BMS_FRAME_P;

#pragma pack(pop)

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_battery
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_battery_init(void);

/*****************************************************************
功  能: 释放 bsp_battery 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_battery_deInit(void);

int      bsp_battery_bms_update_it(uint32_t timeout_ms);
int      bsp_battery_bms_update(uint32_t timeout_ms);
uint32_t bsp_battery_volatage_percent_get(void);
uint32_t bsp_battery_charge_sta_get(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_BATTERY */

#endif
