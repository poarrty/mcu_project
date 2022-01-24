/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_temp.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 19:42:12
 * @Description: 温度传感器接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_TEMP_H__
#define __FAL_TEMP_H__

#include <stdint.h>

void read_temperature_data(uint8_t *register_val, uint16_t len, uint16_t index);

#endif
