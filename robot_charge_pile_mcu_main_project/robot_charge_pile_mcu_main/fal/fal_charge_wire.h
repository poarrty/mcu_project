/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_charge_wire.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-22 10:40:02
 * @Description: 有线充电头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_CHARGE_WIRE_H__
#define __FAL_CHARGE_WIRE_H__

#include <stdint.h>

typedef struct {
    //有线充电相关标志位
    uint8_t flag;
} charge_wire_charge_var_stu_t;

typedef enum {
    //有线充电按键触发
    CHARGE_WIRE_KEY_TRIGGER_BIT = 0,
    //有线充电通信触发
    CHARGE_WIRE_COM_TRIGGER_BIT,
} charge_wire_charge_flag_bit_enum_t;

extern charge_wire_charge_var_stu_t wire_charge_var;

void fal_charge_wire_charge_init(void);
void fal_charge_wire_charge_server(void);

#endif
