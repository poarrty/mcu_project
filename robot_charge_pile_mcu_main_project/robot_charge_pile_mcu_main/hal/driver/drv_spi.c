/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_spi.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 14:51:55
 * @Description: SPI驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_spi.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_spi1_send_data
 * @Description: SPI1发送函数
 * @Input: data：要发送的数据地址
 *         len：数据长度
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {uint8_t} *data
 * @param {uint16_t} len
 *******************************************************************************/
HAL_StatusTypeDef drv_spi1_send_data(uint8_t *data, uint16_t len) {
    HAL_StatusTypeDef status;

    status = HAL_SPI_Transmit(&hspi1, data, len, 1000);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}
