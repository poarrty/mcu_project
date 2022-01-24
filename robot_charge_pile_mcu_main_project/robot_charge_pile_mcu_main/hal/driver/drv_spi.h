/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_spi.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 14:51:55
 * @Description: SPI驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "spi.h"
#include <stdint.h>

HAL_StatusTypeDef drv_spi1_send_data(uint8_t *data, uint16_t len);

#endif
