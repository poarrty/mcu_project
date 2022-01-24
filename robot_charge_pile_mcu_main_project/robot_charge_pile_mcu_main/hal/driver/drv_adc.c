/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_adc.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 15:28:59
 * @Description: ADC驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_adc.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_adc_init
 * @Description: ADC底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 *******************************************************************************/
HAL_StatusTypeDef drv_adc_init(void) {
    HAL_StatusTypeDef status;

    status = HAL_ADCEx_Calibration_Start(&hadc1);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
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
    ADC_ChannelConfTypeDef adc1_channel_config;
    HAL_StatusTypeDef status;

    adc1_channel_config.Channel = ADC_CHANNEL_14;
    adc1_channel_config.Rank = ADC_REGULAR_RANK_1;
    adc1_channel_config.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    status = HAL_ADC_ConfigChannel(&hadc1, &adc1_channel_config);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_ADC_Start(&hadc1);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_ADC_PollForConversion(&hadc1, 100);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return (uint16_t) HAL_ADC_GetValue(&hadc1);
}
