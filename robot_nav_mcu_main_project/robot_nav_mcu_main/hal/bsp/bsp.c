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
#include "bsp.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "stdio.h"

/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup Robot_BSP  - BSP
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
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_init
 * Description:    初始化 bsp
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
int bsp_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_deInit
 * Description:    释放 bsp 资源
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
int bsp_deInit(void) {
    return 0;
}

uint8_t debug_uart_choose(char *str) {
    if (strcmp(str, "RK3399") == 0) {
        HAL_GPIO_WritePin(MX_NVG_UART_SW1_GPIO_Port, MX_NVG_UART_SW1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MX_NVG_UART_SW2_GPIO_Port, MX_NVG_UART_SW2_Pin, GPIO_PIN_RESET);
    } else if (strcmp(str, "MT7621") == 0) {
        HAL_GPIO_WritePin(MX_NVG_UART_SW1_GPIO_Port, MX_NVG_UART_SW1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MX_NVG_UART_SW2_GPIO_Port, MX_NVG_UART_SW2_Pin, GPIO_PIN_RESET);
    } else if (strcmp(str, "MCU103") == 0) {
        HAL_GPIO_WritePin(MX_NVG_UART_SW1_GPIO_Port, MX_NVG_UART_SW1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MX_NVG_UART_SW2_GPIO_Port, MX_NVG_UART_SW2_Pin, GPIO_PIN_SET);
    } else {
        printf("parameters error.");
        return 1;
    }

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), debug_uart_choose, debug_uart_choose, debug_uart_choose);

#ifdef __cplusplus
}
#endif

/* @} Robot_BSP */
/* @} Robot_HAL */
