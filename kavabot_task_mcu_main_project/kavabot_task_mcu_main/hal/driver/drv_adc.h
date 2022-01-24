/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_adc.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 15:07:15
 * @Description: ADC底层驱动接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include <stdint.h>

void drv_adc_init(void);
uint16_t drv_adc_1_channel_0_get_convert_val(void);
uint16_t drv_get_temp_adc_convert_value(void);

#endif
