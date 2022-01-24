/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_modbus.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 15:47:46
 * @Description: MODBUS协议抽象层文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "pal_modbus.h"
#include "mb.h"
#include "fal_power.h"
#include "fal_version.h"
#include "bsp_s09.h"
#include "bsp_e2prom.h"
#include "fal_temp.h"
#include "drv_rtc.h"
#include "drv_gpio.h"
#include <string.h>
#include "common_def.h"
#include "log.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

modbus_cmd_ops_stu_t input_reg[] = {
    {MODBUS_INPUT_REGISTER_DEFINE(0x30A0), 2, NULL, read_power_off_request},
    {MODBUS_INPUT_REGISTER_DEFINE(0x30D0), 4, NULL, read_temperature_data},
    {MODBUS_INPUT_REGISTER_DEFINE(0x30FC), 10, NULL, read_uitrasonic_data},
    {MODBUS_INPUT_REGISTER_DEFINE(0x3308), 40, NULL, read_mcu_version_info},
    {MODBUS_INPUT_REGISTER_DEFINE(0x3400), 12, NULL, read_rtc_data},
    {MODBUS_INPUT_REGISTER_DEFINE(0x3500), 4, NULL, read_periphral_status},
};

modbus_cmd_ops_stu_t hold_reg[] = {
    {MODBUS_HOLD_REGISTER_DEFINE(0x4132), 2, write_power_on_or_off_respond,
     NULL},
    {MODBUS_HOLD_REGISTER_DEFINE(0x4200), 8, write_rtc_data, NULL},
    {MODBUS_HOLD_REGISTER_DEFINE(0x4304), 2, write_heat_ctl, NULL},
};

/******************************************************************************
 * @Function: pal_modbus_init
 * @Description: MODBUS初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void pal_modbus_init(void) {
    eMBInit(MB_RTU, MODBUS_SLAVE_ADDR, 0x02, 115200, MB_PAR_NONE);
    eMBSetSlaveID(MODBUS_SLAVE_ADDR, TRUE, NULL, 0);
    eMBEnable();
}

/******************************************************************************
 * @Function: eMBRegInputCB
 * @Description: 读输入寄存器处理函数
 * @Input: pucRegBuffer：存放要读取得数据的地址
 *         usAddress：读取的寄存器地址
 *         usNRegs：要读取的寄存器的个数
 * @Output: None
 * @Return: void
 * @Others: 对应功能码04
 * @param {UCHAR} *pucRegBuffer
 * @param {USHORT} usAddress
 * @param {USHORT} usNRegs
 *******************************************************************************/
eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress,
                           USHORT usNRegs) {
    eMBErrorCode eStatus = MB_ENOERR;
    uint8_t i = 0;
    uint8_t index = 0;

    for (i = 0; i < ARRAY_SIZE(input_reg); i++) {
        if ((usAddress >= input_reg[i].reg) &&
            ((usAddress + usNRegs) <=
             (input_reg[i].reg + input_reg[i].reg_max_len))) {
            index = usAddress - input_reg[i].reg;

            LOG_DEBUG("mb_Addr:%#X, mb_Nreg:%d, md_index:%d", usAddress,
                      usNRegs, index);

            input_reg[i].read(pucRegBuffer, 2 * usNRegs, 2 * index);
            eStatus = MB_ENOERR;
            break;
        } else {
            eStatus = MB_ENOREG;
        }
    }

    return eStatus;
}

/******************************************************************************
 * @Function: eMBRegHoldingCB
 * @Description: 保持寄存器处理函数
 * @Input: pucRegBuffer：读写数据地址
 *         usAddress：读写寄存器地址
 *         usNRegs：读写寄存器个数
 *         eMode：读写模式
 * @Output: None
 * @Return: void
 * @Others: 对应功能码03、06、16
 * @param {UCHAR} *pucRegBuffer
 * @param {USHORT} usAddress
 * @param {USHORT} usNRegs
 * @param {eMBRegisterMode} eMode
 *******************************************************************************/
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                             USHORT usNRegs, eMBRegisterMode eMode) {
    eMBErrorCode eStatus = MB_ENOERR;
    uint8_t i = 0;
    uint8_t index = 0;

    for (i = 0; i < ARRAY_SIZE(hold_reg); i++) {
        if ((usAddress >= hold_reg[i].reg) &&
            (usAddress <= hold_reg[i].reg + hold_reg[i].reg_max_len)) {
            switch (eMode) {
                case MB_REG_READ:
                    index = usAddress - hold_reg[i].reg;

                    LOG_DEBUG("mb_Addr:%#X, mb_Nreg:%d, md_index:%d", usAddress,
                              usNRegs, index);

                    hold_reg[i].read(pucRegBuffer, 2 * usNRegs, 2 * index);
                    eStatus = MB_ENOERR;
                    break;

                case MB_REG_WRITE:
                    index = usAddress - hold_reg[i].reg;

                    LOG_DEBUG("mb_Addr:%#X, mb_Nreg:%d, md_index:%d", usAddress,
                              usNRegs, index);

                    hold_reg[i].write(pucRegBuffer, 2 * usNRegs, 2 * index);
                    eStatus = MB_ENOERR;
                    break;
            }
        } else {
            eStatus = MB_ENOREG;
        }
    }

    return eStatus;
}

/******************************************************************************
 * @Function: eMBRegCoilsCB
 * @Description: 线圈寄存器处理函数
 * @Input: pucRegBuffer：读写数据地址
 *         usAddress：读写寄存器地址
 *         usNRegs：读写寄存器个数
 *         eMode：读写模式
 * @Output: None
 * @Return: void
 * @Others: 对应功能码01、05、15
 * @param {UCHAR} *pucRegBuffer
 * @param {USHORT} usAddress
 * @param {USHORT} usNCoils
 * @param {eMBRegisterMode} eMode
 *******************************************************************************/
eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                           USHORT usNCoils, eMBRegisterMode eMode) {
    eMBErrorCode eStatus = MB_ENOERR;

    return eStatus;
}

/******************************************************************************
 * @Function: eMBRegDiscreteCB
 * @Description:
 * @Input: pucRegBuffer：存放要读取得数据的地址
 *         usAddress：读取的寄存器地址
 *         usNRegs：要读取的寄存器的个数
 * @Output: None
 * @Return: void
 * @Others: 对应功能码02
 * @param {UCHAR} *pucRegBuffer
 * @param {USHORT} usAddress
 * @param {USHORT} usNDiscrete
 *******************************************************************************/
eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress,
                              USHORT usNDiscrete) {
    eMBErrorCode eStatus = MB_ENOERR;

    return eStatus;
}

/******************************************************************************
 * @Function: read_periphral_status
 * @Description: 获取外设状态
 * @Input: register_val：存放读取数据的地址
 *         len：要读取得数据长度
 *         index：起始索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint8_t} len
 * @param {uint8_t} index
 *******************************************************************************/
void read_periphral_status(uint8_t *register_val, uint16_t len,
                           uint16_t index) {
#if __ABNORMAL_TEST_EN__
    global_var.peripheral_status[1] |= (1 << 3);
#endif

    LOG_DEBUG_ARRAY(global_var.peripheral_status,
                    ARRAY_SIZE(global_var.peripheral_status));

    while (len--) { *register_val++ = global_var.peripheral_status[index++]; }
}

/******************************************************************************
 * @Function: write_heat_ctl
 * @Description: 加热控制
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint16_t} len
 * @param {uint16_t} index
 *******************************************************************************/
void write_heat_ctl(uint8_t *register_val, uint16_t len, uint16_t index) {
    uint8_t buff[2] = {0};

    while (len--) { buff[index++] = *register_val++; }

    if (buff[1]) {
        HEAT_CTL_ON();
    } else {
        HEAT_CTL_OFF();
    }
}
