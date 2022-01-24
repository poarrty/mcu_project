/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_version.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 18:09:49
 * @Description: 版本信息头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_VERSION_H__
#define __FAL_VERSION_H__

#include <stdint.h>

void fal_version_init(void);
void fal_version_info_display(void);
void read_mcu_version_info(uint8_t *register_val, uint16_t len, uint16_t index);

#endif
