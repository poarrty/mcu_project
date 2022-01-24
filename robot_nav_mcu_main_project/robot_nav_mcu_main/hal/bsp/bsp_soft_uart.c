/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-6
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_soft_uart.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "elog.h"
#include "tim.h"
#include "fal_cliff.h"
#include "delay.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_SOFT_UART - SOFT_UART
 *
 * @brief  \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
uint8_t  soft_uart_buff[128] = {0};
uint32_t pin_data[90];

/*****************************************************************
 * 外部变量声明
 ******************************************************************/
extern DMA_HandleTypeDef hdma_tim8_ch4_trig_com;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/
static void UART_Emul_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void UART_Emul_DMAReceiveError(DMA_HandleTypeDef *hdma);
static void UART_Emul_ReceiveFrame(DMA_HandleTypeDef hdma_rx);

/*****************************************************************/
/**
 * Function:       bsp_soft_uart_init
 * Description:    初始化 bsp_soft_uart
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int bsp_soft_uart_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_soft_uart_deInit
 * Description:    释放 bsp_soft_uart 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int bsp_soft_uart_deInit(void) {
    return 0;
}

typedef enum {
    BIT_START = 0,
    BIT_0,
    BIT_1,
    BIT_2,
    BIT_3,
    BIT_4,
    BIT_5,
    BIT_6,
    BIT_7,
    BIT_STOP,

} SOFT_UART_STA_E;

void bsp_soft_uart_handle(void) {
    // static uint8_t buff[256] = {0};
    // static uint16_t cnt = 0;

    static SOFT_UART_STA_E sta = BIT_START;
    static uint8_t         temp;

    switch (sta) {
        case BIT_START:

            temp = 0;

            ///< 确认是起始位
            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_RESET) {
                HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

                /*
                HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
                __HAL_TIM_CLEAR_IT(&SOFT_UART_TIM_H, TIM_IT_UPDATE);
                __HAL_TIM_SET_COUNTER(&SOFT_UART_TIM_H, 0);
                __HAL_TIM_SET_AUTORELOAD(&SOFT_UART_TIM_H, 24);
                HAL_TIM_Base_Start_IT(&SOFT_UART_TIM_H);

                sta = BIT_0;
                */
                UART_Emul_ReceiveFrame(hdma_tim8_ch4_trig_com);
            }

            break;

        case BIT_0:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 0;
            }

            HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
            __HAL_TIM_CLEAR_IT(&SOFT_UART_TIM_H, TIM_IT_UPDATE);
            __HAL_TIM_SET_COUNTER(&SOFT_UART_TIM_H, 0);
            __HAL_TIM_SET_AUTORELOAD(&SOFT_UART_TIM_H, 14);
            HAL_TIM_Base_Start_IT(&SOFT_UART_TIM_H);

            sta = BIT_1;

            break;

        case BIT_1:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 1;
            }

            sta = BIT_2;

            break;

        case BIT_2:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 2;
            }

            sta = BIT_3;

            break;

        case BIT_3:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 3;
            }

            sta = BIT_4;

            break;

        case BIT_4:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 4;
            }

            sta = BIT_5;

            break;

        case BIT_5:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 5;
            }

            sta = BIT_6;

            break;

        case BIT_6:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 6;
            }

            sta = BIT_7;

            break;

        case BIT_7:

            if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                temp |= 0x01 << 7;
            }

            sta = BIT_STOP;

            break;

        case BIT_STOP:

            HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
            __HAL_TIM_CLEAR_IT(&SOFT_UART_TIM_H, TIM_IT_UPDATE);

            /*
            if(cnt < sizeof(buff))
            {
                buff[cnt] = temp;
                cnt++;
            }
            */

            lwrb_write(&cliff_rbuff, &temp, 1);

            ///< 等待停止位
            while (1) {
                if (HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port, MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) {
                    break;
                }
            }

            sta = BIT_START;

            HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

            HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

            break;

        default:

            sta = BIT_START;
            HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
            HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

            break;
    }
}

static void UART_Emul_DMAReceiveCplt(DMA_HandleTypeDef *hdma) {
    // static uint8_t buff[256] = {0};
    // static uint16_t cnt = 0;

    // bsp_soft_uart_handle();
    uint8_t i, j, data;

    for (i = 0; i < 1; i++) {
        data = 0;

        for (j = 0; j < 8; j++) {
            if (pin_data[i * 10 + (j + 1)] & MX_LUNA_UART_SOFT_RX_Pin) {
                data |= 0x01 << j;
            }
        }

        lwrb_write(&cliff_rbuff, &data, 1);
    }

    HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
    __HAL_TIM_CLEAR_IT(&SOFT_UART_TIM_H, TIM_IT_UPDATE);

    ///< 等待停止位
    /*
    while(1)
    {
        if(HAL_GPIO_ReadPin(MX_LUNA_UART_SOFT_RX_GPIO_Port,
    MX_LUNA_UART_SOFT_RX_Pin) == GPIO_PIN_SET) break;
    }
    */

    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void UART_Emul_DMAReceiveError(DMA_HandleTypeDef *hdma) {
    HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
    __HAL_TIM_CLEAR_IT(&SOFT_UART_TIM_H, TIM_IT_UPDATE);

    ///< 恢复起始位检测
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

volatile uint16_t arr_val = 1300;
volatile uint16_t cc_val  = 600;

uint8_t luna_uart_para(uint16_t arr, uint16_t cc) {
    arr_val = arr;
    cc_val  = cc;

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), luna_uart_para, luna_uart_para, luna_uart_para);

static void UART_Emul_ReceiveFrame(DMA_HandleTypeDef hdma_rx) {
    uint32_t tmp_sr   = 0;
    uint32_t tmp_ds   = 0;
    uint32_t tmp_size = 0;
    // uint32_t tmp_arr =0;

    tmp_ds   = (uint32_t) pin_data;
    tmp_sr   = (uint32_t) & (MX_LUNA_UART_SOFT_RX_GPIO_Port->IDR);
    tmp_size = 10 * 1;

    HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM_H);
    __HAL_TIM_SET_COUNTER(&SOFT_UART_TIM_H, 0);
    __HAL_TIM_SET_AUTORELOAD(&SOFT_UART_TIM_H, arr_val);
    SOFT_UART_TIM_I->CCR4 = cc_val;

    hdma_tim8_ch4_trig_com.XferCpltCallback  = UART_Emul_DMAReceiveCplt;
    hdma_tim8_ch4_trig_com.XferErrorCallback = UART_Emul_DMAReceiveError;

    /* Enable the transfer complete interrupt */
    __HAL_DMA_ENABLE_IT(&hdma_rx, DMA_IT_TC);

    /* Enable the transfer Error interrupt */
    __HAL_DMA_ENABLE_IT(&hdma_rx, DMA_IT_TE);

    /* Configure DMA Stream data length */
    hdma_rx.Instance->NDTR = tmp_size;

    /* Configure DMA Stream source address */
    hdma_rx.Instance->PAR = tmp_sr;

    /* Configure DMA Stream destination address */
    hdma_rx.Instance->M0AR = tmp_ds;

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&hdma_rx);

    /* Enable the TIM Update DMA request */
    __HAL_TIM_ENABLE_DMA(&SOFT_UART_TIM_H, TIM_DMA_CC4);

    /* Enable Timer */
    __HAL_TIM_ENABLE(&SOFT_UART_TIM_H);
}

#ifdef __cplusplus
}
#endif

/* @} BSP_SOFT_UART */
/* @} Robot_BSP */
