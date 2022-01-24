/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_battery.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-17 21:03:39
 * @Description: 电池任务头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_BATTERY_H__
#define __TASK_BATTERY_H__

#include "can.h"
#include <stdint.h>

// BMS通信请求CAN ID
#ifndef BATTERY_BMS_REQUEST_CAN_ID
#define BATTERY_BMS_REQUEST_CAN_ID 0X0000052D
#endif

// BMS通信响应CAN ID
#ifndef BATTERY_BMS_RESPOND_CAN_ID
#define BATTERY_BMS_RESPOND_CAN_ID 0X00000080
#endif

// BMS通信首帧标识
#ifndef BATTERY_BMS_PACK_INDEX_START
#define BATTERY_BMS_PACK_INDEX_START 0x80
#endif

// BMS通信尾帧标识
#ifndef BATTERY_BMS_PACK_INDEX_END
#define BATTERY_BMS_PACK_INDEX_END 0x41
#endif

//电池信息最大长度
#ifndef BATTERY_INFO_MAX_LEN
#define BATTERY_INFO_MAX_LEN 256
#endif

//电池信息起始地址
#ifndef BATTERY_INFO_START_ADDR
#define BATTERY_INFO_START_ADDR 0x10
#endif

//电池信息终止地址
#ifndef BATTERY_INFO_END_ADDR
#define BATTERY_INFO_END_ADDR 0x31
#endif

//电池设备通信地址
#ifndef BATTERY_DEVICE_ADDR
#define BATTERY_DEVICE_ADDR 0x06
#endif

//读取电池信息的功能码
#ifndef BATTERY_READ_FUNC_CODE
#define BATTERY_READ_FUNC_CODE 0x05
#endif

//获取电池信息超时时间
#ifndef BATTERY_INFO_REQUEST_TIMEOUT_CNT
#define BATTERY_INFO_REQUEST_TIMEOUT_CNT 1000
#endif

//数据定义：0：正常  1：保护
typedef struct {
    //电芯低压
    uint16_t cell_v_low : 1;
    //电芯高压
    uint16_t cell_v_hight : 1;
    //总压低压
    uint16_t total_v_low : 1;
    //总压高压
    uint16_t total_v_hight : 1;
    //一级放电过流
    uint16_t first_current_out_over_flow : 1;
    //二级放电过流
    uint16_t second_current_out_over_flow : 1;
    //短路
    uint16_t short_connect : 1;
    //一级充电过流
    uint16_t first_current_in_over_flow : 1;
    //二级充电过流
    uint16_t second_current_in_over_flow : 1;
    //放电高温
    uint16_t current_out_temp_hight : 1;
    //放电低温
    uint16_t current_out_temp_low : 1;
    //充电高温
    uint16_t current_in_temp_hight : 1;
    //充电低温
    uint16_t current_in_temp_low : 1;
    // MOS高温
    uint16_t mos_temp_hight : 1;
    uint16_t reserve_bit1 : 1;
    uint16_t reserve_bit2 : 1;
} battery_info_protect_status_byte_stu_t;

//数据定义：0：正常  1：告警
typedef struct {
    //电芯低压
    uint16_t cell_v_low : 1;
    //电芯高压
    uint16_t cell_v_hight : 1;
    //总压低压
    uint16_t total_v_low : 1;
    //总压高压
    uint16_t total_v_hight : 1;
    //放电过流
    uint16_t current_out_over_flow : 1;
    //充电过流
    uint16_t current_in_over_flow : 1;
    //电流采集
    uint16_t current_sample : 1;
    //放电高温
    uint16_t current_out_temp_hight : 1;
    //放电低温
    uint16_t current_out_temp_low : 1;
    //充电高温
    uint16_t current_in_temp_hight : 1;
    //充电低温
    uint16_t current_in_temp_low : 1;
    // MOS高温
    uint16_t mos_temp_hight : 1;
    //低容量
    uint16_t cap_low : 1;
    // AFE采集
    uint16_t afe_sample : 1;
    uint16_t reserve_bit1 : 1;
    uint16_t reserve_bit2 : 1;
} battery_info_alarm_status_byte_stu_t;

typedef struct {
    //放电管：0：关闭  1：开启
    uint16_t disrharge_tube : 1;
    //充电管：0：关闭  1：开启
    uint16_t charge_tube : 1;
    uint16_t reserve_bit1 : 1;
    uint16_t reserve_bit2 : 1;
    //放电状态：0：无  1：放电
    uint16_t disrharge_sta : 1;
    //充电状态：0：无  1：充电
    uint16_t charge_sta : 1;
    uint16_t reserve_bit3 : 1;
    uint16_t reserve_bit4 : 1;
    uint16_t reserve_bit5 : 1;
    uint16_t reserve_bit6 : 1;
    uint16_t reserve_bit7 : 1;
    uint16_t reserve_bit8 : 1;
    uint16_t reserve_bit9 : 1;
    uint16_t reserve_bit10 : 1;
    uint16_t reserve_bit11 : 1;
    uint16_t reserve_bit12 : 1;
} battery_info_bms_status_byte_stu_t;

typedef struct {
    //第1串均衡
    uint16_t strand1 : 1;
    //第2串均衡
    uint16_t strand2 : 1;
    //第3串均衡
    uint16_t strand3 : 1;
    //第4串均衡
    uint16_t strand4 : 1;
    //第5串均衡
    uint16_t strand5 : 1;
    //第6串均衡
    uint16_t strand6 : 1;
    //第7串均衡
    uint16_t strand7 : 1;
    //第8串均衡
    uint16_t strand8 : 1;
    //第9串均衡
    uint16_t strand9 : 1;
    //第10串均衡
    uint16_t strand10 : 1;
    //第11串均衡
    uint16_t strand11 : 1;
    //第12串均衡
    uint16_t strand12 : 1;
    //第13串均衡
    uint16_t strand13 : 1;
    //第14串均衡
    uint16_t strand14 : 1;
    //第15串均衡
    uint16_t strand15 : 1;
    //第16串均衡
    uint16_t strand16 : 1;
} battery_info_balance_status_byte_stu_t;

#pragma pack(push)
#pragma pack(1)

typedef struct {
    //电流
    int32_t current;
    //满充容量
    uint32_t cap_full_in;
    //满放容量
    uint32_t cap_full_out;
    //剩余容量
    uint32_t cap_remain;
    //总电压
    uint16_t voltage;
    // RSOC
    uint16_t rsos;
    //循环次数
    uint16_t cyc_time;
    //保护状态字节
    battery_info_protect_status_byte_stu_t sta_protect;
    //告警状态字节
    battery_info_alarm_status_byte_stu_t sta_warming;
    // BMS状态字节
    battery_info_bms_status_byte_stu_t sta_bms;
    //均衡状态字节
    battery_info_balance_status_byte_stu_t sta_bla;
    //电芯温度1
    int16_t temp_cell1;
    //电芯温度2
    int16_t temp_cell2;
    //电芯温度3
    int16_t temp_cell3;
    //电芯温度4
    int16_t temp_cell4;
    //环境温度
    int16_t temp_env;
    //功率板1温度
    int16_t temp_power_board1;
    //功率板2温度
    int16_t temp_power_board2;
    // cell1电压
    uint16_t vol_cell1;
    // cell2电压
    uint16_t vol_cell2;
    // cell3电压
    uint16_t vol_cell3;
    // cell4电压
    uint16_t vol_cell4;
    // cell5电压
    uint16_t vol_cell5;
    // cell6电压
    uint16_t vol_cell6;
    // cell7电压
    uint16_t vol_cell7;
    // cell8电压
    uint16_t vol_cell8;
    // cell9电压
    uint16_t vol_cell9;
    // cell10电压
    uint16_t vol_cell10;
    // cell11电压
    uint16_t vol_cell11;
    // cell12电压
    uint16_t vol_cell12;
    // cell13电压
    uint16_t vol_cell13;
    // cell14电压
    uint16_t vol_cell14;
    // cell15电压
    // uint16_t vol_cell15;
    // cell16电压
    // uint16_t vol_cell16;
} battery_info_stu_t;

typedef struct {
    uint8_t addr;
    uint8_t len;
    uint8_t func;
    battery_info_stu_t bt_info;
    uint16_t crc;
} battery_bms_frame_stu_t;

typedef struct {
    //电池相关的标志位
    uint8_t flag;
    //等待电池信息超时计数
    uint16_t battery_info_timeout_cnt;
    //电池信息
    battery_info_stu_t battery_info;
    // BMS数据
    uint8_t bms_data[BATTERY_INFO_MAX_LEN];
} battery_var_stu_t;

#pragma pack(pop)

typedef enum {
    //等待电池信息超时使能标志位
    BATTERY_INFO_REQUEST_UPDATE_BIT = 0,
} battery_flag_bit_enum_t;

extern battery_var_stu_t battery_var;

void battery_init(void);
uint8_t get_battery_charge_stutus_bit(void);
uint8_t get_battery_percentage(void);
void battery_server(void);
int task_battery_init(void);
int task_battery_deInit(void);

#endif
