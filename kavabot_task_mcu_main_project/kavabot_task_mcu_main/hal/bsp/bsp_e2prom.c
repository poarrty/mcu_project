/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_e2prom.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-30 17:57:55
 * @Description: E2PROM接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "bsp_e2prom.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "common_def.h"
#include "log.h"
#include <string.h>

/******************************************************************************
 * @Function: bsp_e2prom_init
 * @Description: E2PROM初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void bsp_e2prom_init(void) {
    memset(&global_var, 0, sizeof(global_var_stu_t));
    E2PROM_WP_EN_OFF();

    if (bsp_e2prom_read_write_test()) {
        global_var.peripheral_status[1] |= (1 << 3);
    }
}

/******************************************************************************
 * @Function: bsp_e2prom_write_nbyte
 * @Description: E2PROM写N个字节
 * @Input: start_addr：写入数据存放的起始地址
 *         data：写入数据的地址
 *         len：写入数据的长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint16_t} start_addr
 * @param {uint8_t} *data
 * @param {uint16_t} len
 *******************************************************************************/
void bsp_e2prom_write_nbyte(uint16_t start_addr, uint8_t *data, uint16_t len) {
    while (len--) {
        drv_i2c1_mem_write(
            E2PROM_DEVICE_ADDR | (((start_addr >> 8) & 0xFF) << 1),
            start_addr & 0xFF, I2C_MEMADD_SIZE_8BIT, data, 1);
        start_addr++;
        data++;
        //此处必须要有延时：1.9ms~3ms
        HAL_Delay(3);
    }
}

/******************************************************************************
 * @Function: bsp_e2prom_read_nbyte
 * @Description: 从E2PROM读取N个字节
 * @Input: start_addr：要读取得数据的存放地址
 *         data：接收读取数据的地址
 *         len：读取数据的长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint16_t} start_addr
 * @param {uint8_t} *data
 * @param {uint16_t} len
 *******************************************************************************/
void bsp_e2prom_read_nbyte(uint16_t start_addr, uint8_t *data, uint16_t len) {
    while (len--) {
        drv_i2c1_mem_read(
            E2PROM_DEVICE_ADDR | (((start_addr >> 8) & 0xFF) << 1),
            start_addr & 0xFF, I2C_MEMADD_SIZE_8BIT, data, 1);
        start_addr++;
        data++;
    }
}

/******************************************************************************
 * @Function: bsp_e2prom_read_write_test
 * @Description: E2PROM读写测试接口
 * @Input: void
 * @Output: None
 * @Return: 正确返回0，错误返回1
 * @Others: None
 *******************************************************************************/
uint8_t bsp_e2prom_read_write_test(void) {
    uint8_t write_data[16] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
                              0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
    uint8_t read_data[16] = {0};
    uint16_t i = 0;

    bsp_e2prom_write_nbyte(E2PROM_DATA_START_ADDR, write_data, 16);
    bsp_e2prom_read_nbyte(E2PROM_DATA_START_ADDR, read_data, 16);

    for (i = 0; i < 16; i++) {
        if (read_data[i] != write_data[i]) {
            return 1;
        }
    }

    return 0;
}
