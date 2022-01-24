/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_i2c.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-17 10:17:26
 * @Description: 硬件I2C驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_i2c.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_i2c1_mem_write
 * @Description: I2C1写内存函数
 * @Input: DevAddress：设备地址
 *         MemAddress：内存地址
 *         MemAddSize：内存位宽
 *         pData：存放写入数据的地址
 *         Size：写入字节数
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {uint16_t} DevAddress
 * @param {uint16_t} MemAddress
 * @param {uint16_t} MemAddSize
 * @param {uint8_t} *pData
 * @param {uint16_t} Size
 *******************************************************************************/
HAL_StatusTypeDef drv_i2c1_mem_write(uint16_t DevAddress, uint16_t MemAddress,
                                     uint16_t MemAddSize, uint8_t *pData,
                                     uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, MemAddSize,
                               pData, Size, 1000);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}

/******************************************************************************
 * @Function: drv_i2c1_mem_read
 * @Description: I2C1读取内存函数
 * @Input: DevAddress：设备地址
 *         MemAddress：内存地址
 *         MemAddSize：内存位宽
 *         pData：存放读取数据的地址
 *         Size：读取字节数
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {uint16_t} DevAddress
 * @param {uint16_t} MemAddress
 * @param {uint16_t} MemAddSize
 * @param {uint8_t} *pData
 * @param {uint16_t} Size
 *******************************************************************************/
HAL_StatusTypeDef drv_i2c1_mem_read(uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData,
                                    uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, MemAddSize, pData,
                              Size, 1000);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}
