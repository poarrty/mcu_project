/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_e2prom_i2c.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:54:51
 * @Description: E2PROM底层moniI2C驱动头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_E2PROM_I2C_H__
#define __DRV_E2PROM_I2C_H__

#include <stdint.h>

//延时间隔
#ifndef E2PROM_I2C_DELAY_VAL
#define E2PROM_I2C_DELAY_VAL 4
#endif

//超时时间
#ifndef E2PROM_I2C_TIMEOUT_DELAY_VAL
#define E2PROM_I2C_TIMEOUT_DELAY_VAL 250
#endif

//循环写入间隔
#ifndef E2PROM_I2C_CYCLE_WRITE_DELAY_VAL
#define E2PROM_I2C_CYCLE_WRITE_DELAY_VAL 3
#endif

void drv_e2prom_i2c_write_one_byte(uint8_t device_addr, uint16_t reg_addr,
                                   uint8_t data);
void drv_e2prom_i2c_read_one_byte(uint8_t device_addr, uint16_t reg_addr,
                                  uint8_t *data);

#endif
