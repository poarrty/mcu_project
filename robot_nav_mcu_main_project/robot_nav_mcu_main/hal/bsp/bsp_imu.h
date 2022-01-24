/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-24
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_IMU_H
#define _BSP_IMU_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "usart.h"

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup BSP_IMU
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
#define IMU_USART_H huart5
#define IMU_USART_I UART5

#define IMU_FRAME_LEN_MAX 128   ///< IMU 一帧最长长度
#define IMU_FRAME_HEADER  0X3A  ///< IMU 通信帧头
#define IMU_ADDR          0X01  ///< IMU 器件地址
#define IMU_FRAME_END1    0X0D  ///< IMU 通信结束标志1
#define IMU_FRAME_END2    0X0A  ///< IMU 通信结束标志2

#define IMU_GOTO_COMMAND_MODE 0X06  ///< 命令模式指令
#define IMU_GOTO_STREAM_MODE  0X07  ///< 流模式指令
#define IMU_SET_STREAM_FREQ   0X22  ///< 输出频率模式指令
#define IMU_SET_DEGRAD_OUTPUT 0X24  ///< 角度和角速度单位指令
#define IMU_REPLY_ACK         0X00  ///< IMU 应答 ACK
#define IMU_REPLY_NACK        0X01  ///< IMU 应答 NACK

#define IMU_INDEX_HEADER   0X00
#define IMU_INDEX_SENSOR_L 0X01
#define IMU_INDEX_SENSOR_H 0X02
#define IMU_INDEX_CMD_L    0X03
#define IMU_INDEX_CMD_H    0X04
#define IMU_INDEX_LEN_L    0X05
#define IMU_INDEX_LEN_H    0X06
#define IMU_INDEX_DATA     0X07

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef enum {
    IMU_MODE_CMD,    ///< IMU 命令模式
    IMU_MODE_STREAM  ///< IMU 流模式

} IMU_MODE_E;

typedef enum {
    STREAM_FREQ_5   = 5,    ///< 流模式数据频率 5 Hz
    STREAM_FREQ_10  = 10,   ///< 流模式数据频率 10 Hz
    STREAM_FREQ_50  = 50,   ///< 流模式数据频率 50 Hz
    STREAM_FREQ_100 = 100,  ///< 流模式数据频率 100 Hz
    STREAM_FREQ_250 = 250   ///< 流模式数据频率 250 Hz

} STREAM_FREQ_E;

typedef enum {
    DEGRAD_DEGREE,  ///< 角度制单位
    DEGRAD_RADIAN   ///< 弧度制单位

} DEGRAD_UNIT_E;

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t  header;
    uint16_t sensor_id;
    uint16_t cmd;
    uint16_t data_len;
    uint32_t data;
    uint16_t lrc;
    uint8_t  end1;
    uint8_t  end2;

} IMU_FRAME_SET_T, *IMU_FRMAE_SET_P;

typedef struct {
    uint32_t ts;     ///< 1: 时间戳 (500Hz 更新率,即 0.002s) 即单位是 2ms
    float    acc_x;  ///< 2: 原始的加速度计数据 (g)
    float    acc_y;
    float    acc_z;
    float    acc_x_adj;  ///< 3: 校准后的加速度计数据 (g)
    float    acc_y_adj;
    float    acc_z_adj;
    float    gyro_x;  ///< 4: 原始的陀螺仪数据 (dps (默认) / rad/s)
    float    gyro_y;
    float    gyro_z;
    float    gyro_x_adj_offset;  ///< 5: 静止偏差校准后的陀螺仪数据 (dps (默认) /
                                 ///< rad/s)        ///< 机器人角速度用的是这组角速度
    float gyro_y_adj_offset;
    float gyro_z_adj_offset;
    float gyro_x_adj_coord;  ///< 6: 坐标轴校准后的陀螺仪数据 (dps (默认) / rad/s)
    float gyro_y_adj_coord;
    float gyro_z_adj_coord;
    float quat_w;  ///< 7: 四元数 (归一化单位)
    float quat_x;
    float quat_y;
    float quat_z;
    // float euler_roll;         ///< 8: 欧拉角数据 (degree (默认) / rad)
    // float euler_pitch;
    float euler_pitch;  ///< 安装原因，这两个跟手册是反过来的
    float euler_roll;
    float euler_yaw;

} IMU_DATA_T, *IMU_DATA_P;

typedef struct {
    uint8_t    header;
    uint16_t   sensor_id;
    uint16_t   cmd;
    uint16_t   data_len;
    IMU_DATA_T data;
    uint16_t   lrc;
    uint8_t    end1;
    uint8_t    end2;

} IMU_FRAME_DATA_T, *IMU_FRMAE_DATA_P;

#pragma pack(pop)

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_imu
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_imu_init(void);

/*****************************************************************
功  能: 释放 bsp_imu 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_imu_deInit(void);

void    bsp_imu_reset(void);
uint8_t bsp_imu_set_mode(IMU_MODE_E mode);
uint8_t bsp_imu_set_stream_freq(STREAM_FREQ_E freq);
uint8_t bsp_imu_set_degrad_unit(uint8_t is_radian);

uint16_t bsp_imu_frame_check_sum(uint8_t *data, uint16_t len);
uint8_t  bsp_imu_frame_is_ack(uint8_t *frame);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_IMU */

#endif
