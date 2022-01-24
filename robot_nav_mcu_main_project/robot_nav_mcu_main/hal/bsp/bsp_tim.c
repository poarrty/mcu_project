/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_tim.h"
#include "gpio.h"
#include "string.h"
#include "tim.h"
//#include "bsp_soft_uart.h"
//#include "port.h"
#include "shell.h"
#include "bsp_ir_nec.h"
#include "bsp_simuart_tx.h"
#include "bsp_simuart_rx.h"
/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_TIM  - TIM
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

/*****************************************************************
 * 外部变量声明
 ******************************************************************/
void bsp_soft_uart_handle(void);

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_tim_init
 * Description:    初始化 bsp_tim
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
int bsp_tim_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_tim_deInit
 * Description:    释放 bsp_tim 资源
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
int bsp_tim_deInit(void) {
    return 0;
}

void HAL_TIM_PeriodElapsedCallback_user(TIM_HandleTypeDef *htim) {
    /*
    if(htim->Instance == MODBUS_TIM_TIMEOUT.Instance)
    {
        modbusTimerIRQ();
    }
    if(htim->Instance == SOFT_UART_TIM_I)
    {
        bsp_soft_uart_handle();
    }
    */
    extern SimUart_Tx_TypeDef simuart1;

    if (htim->Instance == simuart1.tim.Instance) {
        bsp_simuart_tx_tim_handler(&simuart1);
    }

    extern SimUart_Rx_TypeDef simuart2;

    if (htim->Instance == simuart2.tim.Instance) {
        bsp_simuart_rx_tim_handler(&simuart2);
    }

    extern SimUart_Rx_TypeDef simuart3;

    if (htim->Instance == simuart3.tim.Instance) {
        bsp_simuart_rx_tim_handler(&simuart3);
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {}

#ifdef __cplusplus
}
#endif

/* @} BSP_TIM */
/* @} Robot_BSP */
