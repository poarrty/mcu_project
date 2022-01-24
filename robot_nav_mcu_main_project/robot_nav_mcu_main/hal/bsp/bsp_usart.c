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
#include "bsp_usart.h"
#include "usart.h"
#include "fal_shell.h"
#include "stdio.h"
#include "shell_port.h"
#include "fal_security_mng.h"
#include "fal_imu.h"
#include "bsp_imu.h"
#include "fal_usound.h"
#include "stdbool.h"
#include "elog.h"
/* ----------------------- Modbus includes ----------------------------------*/
//#include "mb.h"
//#include "port.h"
//#include "mbport.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_USART - USART
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
#define ERROR_BUFF_MAX 100
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
extern FILE *f_shell;
extern FILE *f_log;

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
extern void security_data_handle(uint8_t data);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_usart_init
 * Description:    初始化 bsp_usart
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
int bsp_usart_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_usart_deInit
 * Description:    释放 bsp_usart 资源
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
int bsp_usart_deInit(void) {
    return 0;
}

/*
int fputc(int ch, FILE *f)
{

    if(f == stdout)
    {
        //将printf 的内容往 USART3 发送
        while((SHELL_USART_I->SR & 0X40) == 0);
        SHELL_USART_I->DR = (unsigned char) ch;
    }

    if(f == f_shell)
    {
        //将 shell 的内容往 USART3 发送
        while((SHELL_USART_I->SR & 0X40) == 0);
        SHELL_USART_I->DR = (unsigned char) ch;
    }

    if(f == f_log)
    {
        //将 log 的内容往 USART3 发送
        while((SHELL_USART_I->SR & 0X40) == 0);
        SHELL_USART_I->DR = (unsigned char) ch;
    }

    return (ch);
}
*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    bool bTaskWoken = false;

    ///< shell 串口数据
    if (huart->Instance == SHELL_USART_I) {
        /*
        lwrb_write(&shell_rbuff, &shell_recv_byte, 1);
        HAL_UART_Receive_IT(&huart3, (uint8_t*)&shell_recv_byte, 1);
        */
        if (shell_empty_id != NULL && shell_filled_id != NULL) {
            if (osSemaphoreAcquire(shell_empty_id, 0) == osOK) {
                // produce data
                lwrb_write(&shell_rbuff, &shell_recv_byte, 1);
            }

            osSemaphoreRelease(shell_filled_id);
        }

        HAL_UART_Receive_IT(huart, (uint8_t *) &shell_recv_byte, 1);
    }

    ///< modbus 串口数据
    /*
    if(huart->Instance == MODBUS_PORT.Instance)
    {
        bTaskWoken = pxMBFrameCBByteReceived();

        HAL_UART_Receive_IT(&MODBUS_PORT, &modbufUartRxBuff, 1);
    }
    */

    ///< 安保机器人任务板MCU串口数据
    if (huart->Instance == SECURITY_USART_I) {
        if (security_empty_id != NULL && security_filled_id != NULL) {
            if (osSemaphoreAcquire(security_empty_id, 0) == osOK) {
                // produce data
                lwrb_write(&security_rbuff, &security_recv_byte, 1);

                osSemaphoreRelease(security_filled_id);
            }
        }

        HAL_UART_Receive_IT(huart, (uint8_t *) &security_recv_byte, 1);
    }

    //< 导航板超声MCU串口数据
    if (huart->Instance == USOUND_USART_I) {
        if (usound_empty_id != NULL && usound_filled_id != NULL) {
            if (osSemaphoreAcquire(usound_empty_id, 0) == osOK) {
                // produce data
                lwrb_write(&usound_rbuff, &usound_recv_byte, 1);

                osSemaphoreRelease(usound_filled_id);
            }
        }

        HAL_UART_Receive_IT(huart, (uint8_t *) &usound_recv_byte, 1);
    }

#if 0
    ///< IMU 串口数据
    if (huart->Instance == IMU_USART_I) {
        if (imu_empty_id != NULL && imu_filled_id != NULL) {
            if (osSemaphoreAcquire(imu_empty_id, 0) == osOK) {
                // produce data
                lwrb_write(&imu_rbuff, &imu_recv_byte, 1);

                osSemaphoreRelease(imu_filled_id);
            }
        }

        HAL_UART_Receive_IT(huart, (uint8_t *) &imu_recv_byte, 1);
    }
#endif
    portEND_SWITCHING_ISR(bTaskWoken ? pdTRUE : pdFALSE);
}

extern osSemaphoreId_t microros_tx_empty_id;
void                   HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    bool bTaskWoken = false;
    /*
    if(huart->Instance == MODBUS_PORT.Instance)
    {
        bTaskWoken = pxMBFrameCBTransmitterEmpty();
    }
    */
    if (huart->Instance == USART1) {
        osSemaphoreRelease(microros_tx_empty_id);
    }
    portEND_SWITCHING_ISR(bTaskWoken ? pdTRUE : pdFALSE);
}

void bsp_uart5_idle_callback(void) {
    if (__HAL_UART_GET_IT_SOURCE(&huart5, UART_IT_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart5);
        if (imu_filled_id != NULL) {
            osSemaphoreRelease(imu_filled_id);
        }
    }
}

const USART_TypeDef *Instance[]                         = {USART1, USART2, USART3, UART4, UART5, USART6};
uint8_t              uart_errorore_buff[ERROR_BUFF_MAX] = {0};
uint8_t              error_times                        = 0;
uint8_t              uart_restart_buff[4]               = {0};

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    uint8_t i = 0;

    /*在 RXNE = 1 的情况下，当移位寄存器中当前正在接收的字准备好传输到 RDR
     *寄存器时，该 位由硬件置 1。如果 USART_CR1 寄存器中 RXNEIE =
     *1，则会生成中断。该位由软件序列清 零（读入 USART_SR 寄存器，然后读入
     *USART_DR 寄存器）。 0：无上溢错误 1：检测到上溢错误*/
    if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE) {
        /*存储ORE发生在哪个串口*/
        for (; i < sizeof(Instance) / sizeof(Instance[0]); i++) {
            if (huart->Instance == Instance[i] && error_times < ERROR_BUFF_MAX) {
                uart_errorore_buff[error_times++] = i + 1;
            }
        }

        /*清除硬件ORE标志位*/
        __HAL_UART_CLEAR_OREFLAG(huart);

        /*DMA模式可以通过HAL_UART_Receive_DMA再次开启接受，会丢失一个字节*/
        if (huart->hdmarx != NULL) {
            HAL_UART_Receive_DMA(huart, uart_restart_buff, 1);
        }
        /*中断模式可以通过HAL_UART_Receive_IT再次开启接受，会丢失一个字节*/
        else {
            HAL_UART_Receive_IT(huart, uart_restart_buff, 1);
        }
    }
}

void uart_ore_printf(void) {
    for (uint8_t i = 0; i < ERROR_BUFF_MAX; i++) {
        if (uart_errorore_buff[i] != 0) {
            log_w("uart%d ore occurs", uart_errorore_buff[i]);
        } else {
            log_d("no ore");
            break;
        }
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), uart_ore_printf, uart_ore_printf, uart_ore_printf);

#ifdef __cplusplus
}
#endif

/* @} BSP_USART */
/* @} Robot_BSP */
