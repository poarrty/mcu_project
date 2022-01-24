/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_rtc.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 17:25:22
 * @Description: RTC驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_rtc.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_rtc_set_time
 * @Description: 设置RTC的时间
 * @Input: void
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {rtc_time_stu_t} tm
 *******************************************************************************/
HAL_StatusTypeDef drv_rtc_set_time(rtc_time_stu_t tm) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_StatusTypeDef status;

    sTime.Hours = tm.hour;
    sTime.Minutes = tm.min;
    sTime.Seconds = tm.sec;

    sDate.Year = tm.year - 2000;
    sDate.Month = tm.mon;
    sDate.Date = tm.day;
    sDate.WeekDay = tm.week;

    status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d\r\n", status);
        return status;
    }

    status = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d\r\n", status);
        return status;
    }

    return status;
}

/******************************************************************************
 * @Function: drv_rtc_get_time
 * @Description: 获取RTC时间
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {rtc_time_stu_t} *tm
 *******************************************************************************/
HAL_StatusTypeDef drv_rtc_get_time(rtc_time_stu_t *tm) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_StatusTypeDef status;

    status = HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    tm->hour = sTime.Hours;
    tm->min = sTime.Minutes;
    tm->sec = sTime.Seconds;

    tm->year = (uint16_t)(sDate.Year + 2000);
    tm->mon = sDate.Month;
    tm->day = sDate.Date;
    tm->week = sDate.WeekDay;

    return status;
}

/******************************************************************************
 * @Function: read_rtc_data
 * @Description: 获取RTC数据
 * @Input: register_val：存放读取数据的地址
 *         len：要读取得数据长度
 *         index：起始索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint8_t} len
 * @param {uint8_t} index
 *******************************************************************************/
void read_rtc_data(uint8_t *register_val, uint16_t len, uint16_t index) {
    uint8_t buff[12] = {0};
    rtc_time_stu_t tm;

#if __ABNORMAL_TEST_EN__
    buff[1] = 0x01;
#else
    buff[1] = drv_rtc_get_time(&tm);
#endif
    buff[2] = tm.year >> 8;
    buff[3] = tm.year & 0xFF;
    buff[4] = tm.mon;
    buff[5] = tm.day;
    buff[6] = tm.week;
    buff[7] = tm.hour;
    buff[8] = tm.min;
    buff[9] = tm.sec;

    LOG_DEBUG_ARRAY(buff, ARRAY_SIZE(buff));

    while (len--) { *register_val++ = buff[index++]; }
}

/******************************************************************************
 * @Function: write_rtc_data
 * @Description: 设置RTC时间
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint16_t} len
 * @param {uint16_t} index
 *******************************************************************************/
void write_rtc_data(uint8_t *register_val, uint16_t len, uint16_t index) {
    uint8_t buff[8] = {0};
    rtc_time_stu_t tm;

    while (len--) { buff[index++] = *register_val++; }

    tm.year = buff[0] << 8 | buff[1];
    tm.mon = buff[2];
    tm.day = buff[3];
    tm.week = buff[4];
    tm.hour = buff[5];
    tm.min = buff[6];
    tm.sec = buff[7];

    drv_rtc_set_time(tm);
}
