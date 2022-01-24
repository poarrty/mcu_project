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
#include "bsp_exit.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "bsp_ir_nec.h"
#include "bsp_simuart_rx.h"
#include "elog.h"
/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_EXTI  - 外部中断处理
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

/*****************************************************************
 * 外部变量声明
 ******************************************************************/
extern void pal_uros_print_ts(void);

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_exit_init
 * Description:    初始化 bsp_exit
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
int bsp_exit_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_exit_deInit
 * Description:    释放 bsp_exit
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
int bsp_exit_deInit(void) {
    return 0;
}

void bsp_soft_uart_handle(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == MX_LUNA_UART_SOFT_RX_Pin) {
        // bsp_soft_uart_handle();
    }

    extern SimUart_Rx_TypeDef simuart2;

    if (GPIO_Pin == simuart2.gpio_pin) {
        bsp_simuart_rx_exti_handle(&simuart2);
    }

    extern SimUart_Rx_TypeDef simuart3;

    if (GPIO_Pin == simuart3.gpio_pin) {
        bsp_simuart_rx_exti_handle(&simuart3);
    }
}
#ifdef __cplusplus
}
#endif

/* @} BSO_EXIT */
/* @} Robot_BSP */
