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

#ifdef HAL_UART_EMUL_MODULE_ENABLED
#include "stm32f4xx_hal_uart_emul.h"
#endif

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
/*  Enable the clock for port UART Emulation */
#define UART_EMUL_TX_GPIO_CLK_ENABLE() __GPIOD_CLK_ENABLE();
#define UART_EMUL_RX_GPIO_CLK_ENABLE() __GPIOD_CLK_ENABLE();

/* Initialize GPIO and pin number for UART Emulation */
#define UART_EMUL_TX_PIN  GPIO_PIN_8
#define UART_EMUL_TX_PORT GPIOD
#define UART_EMUL_RX_PIN  GPIO_PIN_9
#define UART_EMUL_RX_PORT GPIOD

/* Definition for UART EMUL NVIC */
#define UART_EMUL_EXTI_IRQHandler EXTI9_5_IRQHandler
#define UART_EMUL_EXTI_IRQ        EXTI9_5_IRQn

/* Definition of UART Emulation EXTI line Rx */
#define UART_EMUL_EXTI_RX HAL_GPIO_EXTI_Callback

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
uint8_t soft_uart_buff[128] = {0};

/*****************************************************************
 * 外部变量声明
 ******************************************************************/
UART_Emul_HandleTypeDef UartEmulHandle;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

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
    UartEmulHandle.Init.Mode       = UART_EMUL_MODE_TX_RX;
    UartEmulHandle.Init.BaudRate   = 115200;
    UartEmulHandle.Init.StopBits   = UART_EMUL_STOPBITS_1;
    UartEmulHandle.Init.Parity     = UART_EMUL_PARITY_NONE;
    UartEmulHandle.Init.WordLength = UART_EMUL_WORDLENGTH_8B;

    if (HAL_UART_Emul_Init(&UartEmulHandle) != HAL_OK) {
        log_e("soft uart init error.");
    }

    if (HAL_UART_Emul_Receive_DMA(&UartEmulHandle, (uint8_t *) soft_uart_buff, 128) != HAL_OK) {
        log_e("soft uart receive error.");
    }

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

/**
 * @brief UART Emulation MSP Initialization
 *        This function configures the UART Emulation resources used in this
 * example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - Port for UART Emulation
 *           - Pin Tx and Rx
 * @param  htim: UART Emulation handle pointer
 * @retval None
 */
void HAL_UART_Emul_MspInit(UART_Emul_HandleTypeDef *huart) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #######################*/
    /* Enable clock for UART Emul */
    __UART_EMUL_CLK_ENABLE();

    /* Enable GPIO TX/RX clock */
    UART_EMUL_TX_GPIO_CLK_ENABLE();
    UART_EMUL_RX_GPIO_CLK_ENABLE();

    /* Initialize UART Emulation port name */
    UartEmulHandle.TxPortName = UART_EMUL_TX_PORT;
    UartEmulHandle.RxPortName = UART_EMUL_RX_PORT;

    /*Initialize UART Emulation pin number for Tx */
    UartEmulHandle.Init.RxPinNumber = UART_EMUL_RX_PIN;
    UartEmulHandle.Init.TxPinNumber = UART_EMUL_TX_PIN;

    /* Configure GPIOE for UART Emulation Tx */
    GPIO_InitStruct.Pin   = UART_EMUL_TX_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(UART_EMUL_TX_PORT, &GPIO_InitStruct);

    /* Configure GPIOC for UART Emulation Rx */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pin  = UART_EMUL_RX_PIN;

    HAL_GPIO_Init(UART_EMUL_RX_PORT, &GPIO_InitStruct);

    /*##-2- Enable NVIC for line Rx  #################################*/
    /* Enable and set EXTI Line Interrupt to the highest priority */
    HAL_NVIC_SetPriority(UART_EMUL_EXTI_IRQ, 3, 0);
    HAL_NVIC_EnableIRQ(UART_EMUL_EXTI_IRQ);
}

void UART_EMUL_EXTI_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(UART_EMUL_RX_PIN);
}

void HAL_UART_Emul_RxCpltCallback(UART_Emul_HandleTypeDef *huart) {
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_UART_Emul_TransferComplet could be implemented in the user
       file
     */
    bsp_soft_uart_init();
    HAL_UART_Emul_Receive_DMA(&UartEmulHandle, (uint8_t *) soft_uart_buff, 128);
}

#ifdef __cplusplus
}
#endif

/* @} BSP_SOFT_UART */
/* @} Robot_BSP */
