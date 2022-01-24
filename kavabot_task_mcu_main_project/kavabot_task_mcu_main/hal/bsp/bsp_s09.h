/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_s09.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 16:44:58
 * @Description: 超声传感器驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BSP_S09_H__
#define __BSP_S09_H__

#include <stdint.h>

//超声通道数量
#define UITRASONIC_CHANNEL_NUM 2
//抓取的回波个数
#define UITRASONIC_ECHO_NUM 1
//最大测量范围
#define UITRASONIC_MAX_DISTANCE 1500

typedef struct {
    //超声相关标志位
    uint8_t flag;
    //正在检测的通道编号
    uint8_t current_channel;
    //修正后的时间
    uint16_t fix_time;
    //修正后的距离
    uint16_t fix_distance;
    //控制发送状态机
    uint8_t send_pulse_status;
    //发送脉冲计数
    uint8_t send_pulse_cnt;
    //抓去=取的回波个数
    uint8_t echo_cnt;
    //回波时间
    uint16_t echo_time[UITRASONIC_ECHO_NUM];
    //超声数据上报的数组
    uint8_t distance_val_send_buff[UITRASONIC_CHANNEL_NUM * 2];
    //存放检测到的超声距离数组
    uint16_t distance_val[UITRASONIC_CHANNEL_NUM];
} s09_var_stu_t;

typedef enum {
    UITRASONIC_DETECT_EN_BIT = 0,
    UITRASONIC_RECV_ECHO_FINISH_BIT,
    UITRASONIC_RECV_ECHO_TIMEOUT_BIT,
    UITRASONIC_TIM_IC_FLAG_BIT,
} s09_flag_enum_t;

typedef enum {
    // LOG打印关闭
    LOG_ULTRASONIC_THRES_LOW = 0,
    //打印错误
    LOG_ULTRASONIC_THRES_MIDDLE,
    //打印警告
    LOG_ULTRASONIC_THRES_HIGH
} log_ultrasonicl_thres_enum_t;

extern s09_var_stu_t s09_var;

void bsp_s09_init(void);
void bsp_s09_distance_detect_server(void);
void read_uitrasonic_data(uint8_t *register_val, uint16_t len, uint16_t index);
void bsp_09_channel_loop_thres_setup(void);
void bsp_09_channel_loop_read_thres(void);
void bsp_s09_channel_select(uint8_t channel);
#endif
