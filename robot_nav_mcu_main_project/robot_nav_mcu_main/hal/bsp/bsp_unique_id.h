/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         jianyongxiang
 ** Version:        V0.0.1
 ** Date:           2021-7-2
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-07 jianyongxiang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-7-2      jianyongxiang   0.0.1       创建文件
 ******************************************************************/

#ifndef _BSP_UNIQUE_ID_H
#define _BSP_UNIQUE_ID_H

#define SOC_TYPE_ADDR SOC_STM32F4_ID_ADDR

#define SOC_STM32F0_ID_ADDR 0x1FFFF7AC
#define SOC_STM32F1_ID_ADDR 0x1FFFF7E8
#define SOC_STM32F2_ID_ADDR 0x1FFF7A10
#define SOC_STM32F3_ID_ADDR 0x1FFFF7AC
#define SOC_STM32F4_ID_ADDR 0x1FFF7A10
#define SOC_STM32F7_ID_ADDR 0x1FF0F420
#define SOC_STM32L0_ID_ADDR 0x1FF80050
#define SOC_STM32L1_ID_ADDR 0x1FF80050
#define SOC_STM32L4_ID_ADDR 0x1FFF7590
#define SOC_STM32H7_ID_ADDR 0x1FF0F420

void SocIDGet(uint32_t *id);

#endif
