/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_modbus.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 15:47:12
 * @Description: MODBUS协议抽象层头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __PAL_MODBUS_H__
#define __PAL_MODBUS_H__

#include <stdint.h>

#ifndef MODBUS_SLAVE_ADDR
#define MODBUS_SLAVE_ADDR 0x5A
#endif

#define MODBUS_INPUT_REGISTER_DEFINE(reg) (reg + 1)
#define MODBUS_HOLD_REGISTER_DEFINE(reg) (reg + 1)

typedef struct {
    uint16_t reg;
    uint8_t reg_max_len;
    void (*write)(uint8_t *register_val, uint16_t len, uint16_t index);
    void (*read)(uint8_t *register_val, uint16_t len, uint16_t index);
} modbus_cmd_ops_stu_t;

typedef struct {
    uint16_t reg_addr;
    uint8_t read_write_reg_cnt;
    uint8_t read_write_reg_index;
} modbus_debug_info_stu_t;

void pal_modbus_init(void);
void read_periphral_status(uint8_t *register_val, uint16_t len, uint16_t index);
void write_heat_ctl(uint8_t *register_val, uint16_t len, uint16_t index);

#endif
