#ifndef __BSP_09_H__
#define __BSP_09_H__

#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "log.h"
#include "delay.h"
#include "time.h"
#include "stdio.h"
#include "app.h"
#include "drv_gpio.h"
#include "log.h"
#include "SEGGER_RTT.h"
#include "common_def.h"
#include <stdint.h>
//超声通道数量

#define ELMOS09_CHANNEL_NUM 8

#define ELMOS09_ON 1
#define ELMOS09_OFF 0

extern uint32_t capture_Buf[3];  //存放计数
extern uint8_t capture_Cnt;      //标志位
extern uint32_t high_time;       //一个低电平+高电平时间
extern uint32_t high_time_buff[30];
extern uint8_t receive_flag;

typedef enum {
    // LOG打印关闭
    LOG_ULTRASONIC_THRES_LOW = 0,
    //打印错误
    LOG_ULTRASONIC_THRES_MIDDLE,
    //打印警告
    LOG_ULTRASONIC_THRES_HIGH
} log_ultrasonicl_thres_enum_t;

extern log_ultrasonicl_thres_enum_t log_ultrasonicl_thres;
extern uint8_t digital_ultrasonic_state;
void bsp_09_distance_detect_server();
void bsp_09_distance_detect_server3();
void bsp_09_channel_loop_meas_setup();
void bsp_09_channel_loop_thres_setup();

#endif