/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_i2c.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-17 10:17:07
 * @Description: 硬件I2C驱动接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include <stdint.h>

void drv_i2c1_mem_write(uint16_t DevAddress, uint16_t MemAddress,
                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
void drv_i2c1_mem_read(uint16_t DevAddress, uint16_t MemAddress,
                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

#endif
