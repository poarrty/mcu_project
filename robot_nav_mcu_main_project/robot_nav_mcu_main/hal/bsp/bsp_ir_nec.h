/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-12
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_IR_NEC_H
#define _BSP_IR_NEC_H

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
#define NEC_CMD_CHARGE_PULL_IN  0X10
#define NEC_CMD_CHARGE_RELAY_ON 0X01

#define NEC_CMD_CHECK        0X01
#define NEC_PER_DYNA_CHECK   0X02
#define NEC_PER_STATIC_CHECK 0X03

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef enum {
    NEC_NOT_EXTENDED,
    NEC_EXTENDED

} NEC_TYPE;

typedef enum {
    NEC_INIT,
    NEC_AGC_START_1,
    NEC_AGC_START_2,
    NEC_AGC_START_3,
    NEC_AGC_OK,
    NEC_AGC_FAIL,
    NEC_FAIL,
    NEC_OK

} NEC_STATE;

typedef enum {
    NEC_TX_INIT,
    NEC_TX_AGC,
    NEC_TX_BIT,
    NEC_TX_STOP_A,
    NEC_TX_STOP_B,
    NEC_TX_STOP_C,
    NEC_TX_OK

} NEC_TX_STATE;

typedef struct {
    NEC_TYPE  type;
    NEC_STATE state;

    TIM_HandleTypeDef *   timerHandle;
    uint32_t              timerChannel;
    HAL_TIM_ActiveChannel timerChannelActive;
    GPIO_TypeDef *        rx_port;
    uint16_t              rx_pin;

    uint32_t rawTimerData[32];
    uint8_t  decoded[2];
    uint8_t  cmd;
    uint8_t *cmd_p;
    uint8_t  is_rx_it_mode;

    NEC_TX_STATE       tx_state;
    TIM_HandleTypeDef *tx_pwm_timerHandle;
    TIM_HandleTypeDef *tx_code_timerHandle;
    uint32_t           tx_timerChannel;
    uint8_t            tx_cmd;
    uint8_t            tx_code[2];
    uint8_t            tx_bit_cnt;
    uint8_t            tx_bit_lever;

} NEC_HandleTypeDef;

extern NEC_HandleTypeDef nec1;
extern NEC_HandleTypeDef nec2;
extern NEC_HandleTypeDef nec3;

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
int bsp_ir_nec_init(void);

/*****************************************************************
功  能: 释放 bsp_ir_nec 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_ir_nec_deInit(void);

void bsp_nec_rx_handle(NEC_HandleTypeDef *nec);
void bsp_nec_tx_handle(NEC_HandleTypeDef *nec);

uint8_t NEC_Receive_IT(NEC_HandleTypeDef *nec, uint8_t *cmd);
uint8_t NEC_Receive(NEC_HandleTypeDef *nec, uint8_t *cmd, uint32_t timeout);
uint8_t NEC_Trimamit(NEC_HandleTypeDef *nec, uint8_t cmd, uint8_t check, uint32_t timeout);

uint8_t NEC_Receive_IT_Abort(NEC_HandleTypeDef *nec);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_IR_NEC */

#endif
