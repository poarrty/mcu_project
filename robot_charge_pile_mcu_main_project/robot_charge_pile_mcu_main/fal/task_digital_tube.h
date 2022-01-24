/******************************************************************************
 * Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_digital_tube.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-03-25 21:21:24
 * @Description: 数码管任务相关定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_DIGITAL_TUBE_H__
#define __TASK_DIGITAL_TUBE_H__

#include <stdint.h>

//电量百分比进度条滚动周期
#ifndef DIGITAL_TUBE_DYNAMIC_DISPLAY_CYCLE_CNT
#define DIGITAL_TUBE_DYNAMIC_DISPLAY_CYCLE_CNT 40
#endif

//等待电池百分比数据超时计数
#ifndef DIGITAL_TUBE_DISPLAY_UPDATE_TIMEOUT_CNT
#define DIGITAL_TUBE_DISPLAY_UPDATE_TIMEOUT_CNT 6000
#endif

#define BATTERY_PERCENTAGE_LEVEL_0 0
#define BATTERY_PERCENTAGE_LEVEL_1 12
#define BATTERY_PERCENTAGE_LEVEL_2 25
#define BATTERY_PERCENTAGE_LEVEL_3 37
#define BATTERY_PERCENTAGE_LEVEL_4 50
#define BATTERY_PERCENTAGE_LEVEL_5 62
#define BATTERY_PERCENTAGE_LEVEL_6 75
#define BATTERY_PERCENTAGE_LEVEL_7 87
#define BATTERY_PERCENTAGE_LEVEL_8 100

#define DIGITAL_TUBE_ADD_WATER_START 0xF01             //开始加水
#define DIGITAL_TUBE_ADD_WATER_STOP 0xF02              //停止加水
#define DIGITAL_TUBE_SEWAGE_START 0xF03                //开始排水
#define DIGITAL_TUBE_SEWAGE_STOP 0xF04                 //停止排水
#define DIGITAL_TUBE_TRANSITION_BOX_CLEAN_START 0xF05  //开始冲洗
#define DIGITAL_TUBE_TRANSITION_BOX_CLEAN_STOP 0xF06   //停止冲洗
#define DIGITAL_TUBE_BUTTON_STOP 0xE01                 //停止按键
#define DIGITAL_TUBE_UROS_TIMEOUT 0xE02                // ROS超时

typedef enum {
    DIGITAL_TUBE_0_DISPLAY_EN = 0,
    DIGITAL_TUBE_1_DISPLAY_EN,
    DIGITAL_TUBE_2_DISPLAY_EN,
    DIGITAL_TUBE_3_DISPLAY_EN,
} digital_tube_display_status_enum_t;

typedef enum {
    DYNAMIC_DISPLAY = 0,
    STATIC_DISPLAY = 1,

} digital_tube_display_type_enum_t;

typedef enum {
    //数码管显示电池电量
    BATTERY_DISPLAY = 0,
    //数码管显示功能码
    FUNCTION_CODE_DISPLAY = 1,
    //水位显示
    WATER_LEVEL_DISPLAY = 2,
} digital_tube_display_mode_enum_t;

typedef struct {
    //数码管相关标志位
    uint8_t flag;
    //数码管显示数据
    uint16_t display_data[2];
    //数码管显示数据数量
    uint8_t display_data_num;
    //数码管显示数据类型
    digital_tube_display_mode_enum_t display_data_type[2];
    //电池电量百分比对应的进度条个数
    uint8_t progress_bar_level;
    //上一次电池电量百分比对应的进度条个数
    uint8_t progress_bar_level_pre;
    //每个进度条显示的循环次数
    uint8_t progress_bar_cycle_cnt;
    //等待电池百分比数据超时计数
    uint16_t digital_tube_display_update_timeout_cnt;
    //当前状态
    digital_tube_display_status_enum_t current_status;

} digital_tube_var_stu_t;

typedef enum {
    //数码管显示使能标志位
    DIGITAL_TUBE_DISPLAY_ON_EN_BIT = 0,
    //标记收到电池百分比信息
    DIGITAL_TUBE_DISPLAY_UPDATE_BIT,
    //标记数码管显示类型
    DIGITAL_TUBE_DYNAMIC_DISPLAY_EN_BIT,
} user_digital_tube_flag_bit_enum_t;

extern digital_tube_var_stu_t digital_tube_var;

void digital_tube_init(void);
void digital_tube_display_off(void);
void digital_tube_display_on(uint16_t val,
                             digital_tube_display_type_enum_t type,
                             digital_tube_display_mode_enum_t display_mode);
void digital_tube_display_server(void);
int task_digital_tube_init(void);
int task_digital_tube_deInit(void);

#endif
