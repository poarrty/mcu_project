/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: task_temp.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description: 温度传感器任务入口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __TASK_TEMP_H__
#define __TASK_TEMP_H__

#include "bsp_temp.h"

#ifndef OVER_PROTECT_THRESHOLD_UPPER_LIMIT
#define OVER_PROTECT_THRESHOLD_UPPER_LIMIT 85
#endif

#ifndef OVER_PROTECT_THRESHOLD_LOWER_LIMIT
#define OVER_PROTECT_THRESHOLD_LOWER_LIMIT 60
#endif

typedef struct {
    //温度传感器相关指令
    uint8_t flag;
    //温度传感器相关信息
    temp_info_stu_t info;
} temp_var_stu_t;

typedef enum {
    //过温保护使能标志位
    OVER_PROTECT_EN_BIT = 0,
} temp_flag_bit_enum_t;

extern temp_var_stu_t temp_var;

void temp_init(void);
void over_temp_protect_detect(void);
int task_temp_init(void);
int task_temp_deInit(void);

#endif
