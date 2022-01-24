/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         jianyongxiang
 ** Version:        V0.0.1
 ** Date:           2021-7-30
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-07 jianyongxiang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-7-30       jianyongxiang    0.0.1    创建文件
 ******************************************************************/
#ifndef _BSP_SIMUART_TX_H
#define _BSP_SIMUART_TX_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include <stdint.h>
#include "stm32f4xx_hal.h"

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup bsp_simuart_tx
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef struct {
    uint8_t  SBUF;
    uint8_t  tx_status;
    uint8_t  tx_ptr;
    uint8_t  tx_len;
    uint8_t *buff;

    uint16_t          baudrate;
    GPIO_TypeDef *    gpio_port;
    uint16_t          gpio_pin;
    uint8_t           pwm_mode;
    TIM_HandleTypeDef tx_pwm_tim;
    uint32_t          tx_pwm_channel;
    TIM_HandleTypeDef tim;
    uint32_t          frq_div_clock;
} SimUart_Tx_TypeDef;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_simuart_tx
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_simuart_tx_init(SimUart_Tx_TypeDef *simuartx);

/*****************************************************************
功  能: 释放 bsp_simuart_tx 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_simuart_tx_deInit(void);

void     bsp_simuart_tx_tim_handler(SimUart_Tx_TypeDef *simuartx);
uint32_t bsp_simuart_tx_send_buff(SimUart_Tx_TypeDef *simuartx, uint8_t *buff, uint32_t len);
/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} bsp_simuart_rx */

#endif
