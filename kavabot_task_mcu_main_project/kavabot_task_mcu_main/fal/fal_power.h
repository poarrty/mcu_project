/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_power.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 09:58:23
 * @Description: 电源管理相关头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_POWER_H__
#define __FAL_POWER_H__

#include <stdint.h>

//开机超时时间
#ifndef POWER_ON_RESPOND_WAIT_TIMEOUT_CNT
#define POWER_ON_RESPOND_WAIT_TIMEOUT_CNT 12000
#endif

//关机超时时间
#ifndef POWER_OFF_RESPOND_WAIT_TIMEOUT_CNT
#define POWER_OFF_RESPOND_WAIT_TIMEOUT_CNT 4000
#endif

typedef struct {
    //电源管理相关标志位
    uint8_t flag;
    //开机超时计数
    uint16_t power_on_respond_wait_timeout_cnt;
    //关机超时计数
    uint16_t power_off_respond_wait_timeout_cnt;
} power_var_stu_t;

typedef enum {
    //当前电源状态：0：关机  1：开机
    POWER_CURRENT_STATUS_BIT = 0,
    //开机请求使能
    POWER_ON_REQUEST_EN_BIT,
    //关机请求使能
    POWER_OFF_REQUEST_EN_BIT,
} power_flag_bit_enum_t;

typedef enum {
    POWER_OFF_REQUEST_NO_NEED = 1,
    POWER_OFF_REQUEST_NEED = 2,
} power_off_request_type_enum_t;

typedef enum {
    POWER_OFF_SUCCESS = 1,
    POWER_ON_SUCCESS = 2,
} power_on_or_off_respond_type_enum_t;

extern power_var_stu_t power_var;

void fal_power_init(void);
void fal_sys_power_on(void);
void fal_sys_power_off(void);
void read_power_off_request(uint8_t *register_val, uint16_t len,
                            uint16_t index);
void write_power_on_or_off_respond(uint8_t *register_val, uint16_t len,
                                   uint16_t index);

#endif
