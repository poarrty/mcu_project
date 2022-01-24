/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_e2prom.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-30 17:57:55
 * @Description: E2PROM接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BSP_E2PROM_H__
#define __BSP_E2PROM_H__

#include <stdint.h>

// E2PROM设备地址
#ifndef E2PROM_DEVICE_ADDR
#define E2PROM_DEVICE_ADDR 0xA0
#endif

// E2PROM存放数据的起始地址:0~2047
#ifndef E2PROM_DATA_START_ADDR
#define E2PROM_DATA_START_ADDR 0x00
#endif

void bsp_e2prom_init(void);
void bsp_e2prom_write_nbyte(uint16_t start_addr, uint8_t *data, uint16_t len);
void bsp_e2prom_read_nbyte(uint16_t start_addr, uint8_t *data, uint16_t len);
uint8_t bsp_e2prom_read_write_test(void);

#endif
