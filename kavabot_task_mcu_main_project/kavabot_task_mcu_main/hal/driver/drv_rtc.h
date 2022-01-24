/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_rtc.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 17:25:01
 * @Description: RTC驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_RTC_H__
#define __DRV_RTC_H__

#include "rtc.h"
#include <stdint.h>

typedef struct {
    uint16_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} rtc_time_stu_t;

HAL_StatusTypeDef drv_rtc_set_time(rtc_time_stu_t tm);
HAL_StatusTypeDef drv_rtc_get_time(rtc_time_stu_t *tm);
void read_rtc_data(uint8_t *register_val, uint16_t len, uint16_t index);
void write_rtc_data(uint8_t *register_val, uint16_t len, uint16_t index);

#endif
