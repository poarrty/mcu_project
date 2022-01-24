/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_temp.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:55:18
 * @Description: 温度传感器接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "bsp_temp.h"
#include "drv_adc.h"
#include <math.h>

/******************************************************************************
 * @Function: bsp_get_temp_val
 * @Description: 获取温度传感器的数据
 * @Input: info：传入的存储温度信息结构体的地址
 * @Output: None
 * @Return: void
 * @Others: Rt = Rp * EXP(B * (1/T1 - 1/T2))
 *          T1 = (1 / (log(Rt / Rp) / B + 1/T2)) - K + 0.5(温度补偿)
 * @param {temp_info_stu_t} *info
 *******************************************************************************/
void bsp_get_temp_val(temp_info_stu_t *info) {
    //常数B
    float B = 3380;
    //常温(25℃)下热敏电阻的标称阻值阻值
    float Rp = 10000;
    //分压电阻的阻值
    float Rd = 4700;
    //开尔文温度
    float K = 237.15;
    // 25℃对应的华摄氏度
    float T2 = K + 25;

    info->adc_val = drv_get_temp_adc_convert_value();
    info->vol_val = (float) info->adc_val / 4096 * 3.3;
    info->Rt = (Rd * info->vol_val) / (3.3 - info->vol_val);
    info->T1 = (1 / (1 / T2 + log(info->Rt / Rp) / B)) - K + 0.5;
}
