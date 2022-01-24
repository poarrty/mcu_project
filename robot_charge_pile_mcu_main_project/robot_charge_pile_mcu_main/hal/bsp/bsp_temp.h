/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_temp.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:55:18
 * @Description: 温度传感器接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BSP_TEMP_H__
#define __BSP_TEMP_H__

#include <stdint.h>

typedef struct {
    //采集的ADC值
    uint16_t adc_val;
    //采集的电压值
    float vol_val;
    //当前温度对应的热敏电阻的阻值
    float Rt;
    //当前温度值
    float T1;
} temp_info_stu_t;

void bsp_get_temp_val(temp_info_stu_t *info);

#endif
