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
#ifndef _BSP_SIMUART_RX_H
#define _BSP_SIMUART_RX_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include <stdint.h>
#include "stm32f4xx_hal.h"

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup BSP_IR_NEC
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
#define BUFF_LEN 255
/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef struct {
    uint8_t SBUF;
    uint8_t rx_status;
    uint8_t state;

    uint16_t          baudrate;
    GPIO_TypeDef *    gpio_port;
    uint16_t          gpio_pin;
    TIM_HandleTypeDef tim;
    uint32_t          frq_div_clock;

    uint8_t rx_getpack;
    uint8_t rx_ptr;
    uint8_t rxbuff[BUFF_LEN];
    void (*receive_callback)(uint8_t SBUF);
    uint32_t pack_timeout;
    uint32_t (*get_timecount)(void);
} SimUart_Rx_TypeDef;
/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp_ir_nec
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_simuart_rx_init(SimUart_Rx_TypeDef *simuartx);

/*****************************************************************
功  能: 释放 bsp_ir_nec 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_simuart_rx_deInit(void);

void bsp_simuart_rx_enable(SimUart_Rx_TypeDef *simuartx);
void bsp_simuart_rx_disable(SimUart_Rx_TypeDef *simuartx);
void bsp_simuart_rx_tim_handler(SimUart_Rx_TypeDef *simuartx);
void bsp_simuart_rx_getdata(SimUart_Rx_TypeDef *simuartx, uint8_t *buff, uint8_t *len, uint32_t timeout);
void bsp_simuart_rx_exti_handle(SimUart_Rx_TypeDef *simuartx);
/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_IR_NEC */

#endif
