/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_adc.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 15:07:39
 * @Description: ADC底层驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "adc.h"
#include "drv_adc.h"
#include "common_def.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_adc_init
 * @Description: ADC底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_adc_init(void) {
    Error_Check_Callbake(HAL_ADCEx_Calibration_Start(&hadc2));
}

/******************************************************************************
 * @Function: drv_get_temp_adc_convert_value
 * @Description: 获取温度检测的ADC采样值
 * @Input: void
 * @Output: None
 * @Return: 返回的ADC采样值
 * @Others: 转换周期：(采样周期 + 12.5) * ADC时钟周期
 *******************************************************************************/
uint16_t drv_get_temp_adc_convert_value(void) {
    ADC_ChannelConfTypeDef adc2_channel_config;

    adc2_channel_config.Channel = ADC_CHANNEL_7;
    adc2_channel_config.Rank = ADC_REGULAR_RANK_1;
    adc2_channel_config.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    Error_Check_Callbake(HAL_ADC_ConfigChannel(&hadc2, &adc2_channel_config));
    Error_Check_Callbake(HAL_ADC_Start(&hadc2));
    Error_Check_Callbake(HAL_ADC_PollForConversion(&hadc2, 100));

    return (uint16_t) HAL_ADC_GetValue(&hadc2);
}
