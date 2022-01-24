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
#include "task_shell.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "usart.h"
#include "shell_port.h"
#include "string.h"
#include "lwrb.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup F_SHELL 串口控制台 - SHELL
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
#define SHEEL_RECV_BUFF_SIZE 128

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
uint8_t shell_recv_byte;
lwrb_t shell_rbuff;

osSemaphoreId_t shell_empty_id = NULL;
osSemaphoreId_t shell_filled_id = NULL;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static uint8_t shell_recv_buff[SHEEL_RECV_BUFF_SIZE];

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
 * Function:       fal_shell_init
 * Description:    初始化 fal_shell
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示打开文件成功
 *  - 1 表示打开文件失败
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
int fal_shell_init(void) {
    /*添加模块处理函数*/
    lwrb_init(&shell_rbuff, shell_recv_buff, sizeof(shell_recv_buff));

    userShellInit();

    shell_empty_id =
        osSemaphoreNew(SHEEL_RECV_BUFF_SIZE, SHEEL_RECV_BUFF_SIZE, NULL);
    shell_filled_id = osSemaphoreNew(SHEEL_RECV_BUFF_SIZE, 0U, NULL);

    HAL_UART_Receive_IT(&huart1, (uint8_t *) &shell_recv_byte, 1);

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_shell_deInit
 * Description:    释放 fal_shell 资源
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
int fal_shell_deInit(void) {
    return 0;
}

void task_shell_run(void *argument) {
    uint8_t data;

    for (;;) {
        if (osSemaphoreAcquire(shell_filled_id, osWaitForever) == osOK) {
            // consume data
            if (lwrb_read(&shell_rbuff, &data, 1)) {
                ///< 若关闭调度，则shell
                ///< 的命令函数内不能阻塞等待信号量、互斥锁、消息队列
                // osKernelLock();
                shellHandler(&shell, data);
                // osKernelUnlock();
            }

            osSemaphoreRelease(shell_empty_id);
        }
    }
}

#ifdef __cplusplus
}
#endif

/* @} F_SHELL */
/* @} Robot_FAL */
