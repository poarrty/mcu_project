/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_temp.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 19:42:44
 * @Description: 温度传感器驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_temp.h"
#include "bsp_temp.h"
#include "common_def.h"
#include "log.h"

/******************************************************************************
 * @Function: read_temperature_data
 * @Description: 获取温度传感器的数据
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
void read_temperature_data(uint8_t *register_val, uint16_t len,
                           uint16_t index) {
    uint8_t buff[4] = {0};
    temp_info_stu_t info;

    bsp_get_temp_val(&info);

#if __ABNORMAL_TEST_EN__
    buff[0] = (uint16_t)(-100) >> 8;
    buff[1] = (uint16_t)(-100) & 0xFF;
#else
    buff[0] = ((uint16_t) info.T1) >> 8;
    buff[1] = ((uint16_t) info.T1) & 0xFF;
#endif

    LOG_DEBUG_ARRAY(buff, ARRAY_SIZE(buff));

    while (len--) { *register_val++ = buff[index++]; }
}
