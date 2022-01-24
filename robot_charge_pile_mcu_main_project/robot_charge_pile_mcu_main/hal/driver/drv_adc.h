/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_adc.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 15:28:59
 * @Description: ADC驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "adc.h"
#include <stdint.h>

HAL_StatusTypeDef drv_adc_init(void);
uint16_t drv_get_temp_adc_convert_value(void);

#endif
