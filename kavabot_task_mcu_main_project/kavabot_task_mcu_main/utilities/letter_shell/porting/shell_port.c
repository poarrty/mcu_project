/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell_port.h"

Shell shell;
char shellBuffer[512];
uint8_t letter_shell_recv_buff = 0;

/**
 * @brief 用户shell写
 *
 * @param data 数据
 */
void userShellWrite(char data) {
    HAL_UART_Transmit(&huart1, (uint8_t *) &data, 1, 1000);
}

#if 0
/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @return char 状态
 */
signed char userShellRead(char *data)
{
    if (serialReceive(&debugSerial, (uint8_t *)data, 1, 0) == 1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
#endif

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void) {
    HAL_UART_Receive_IT(&huart1, &letter_shell_recv_buff, 1);
    shell.write = userShellWrite;

#if 0
    shell.read = userShellRead;
#endif

    shellInit(&shell, shellBuffer, 512);
}

/******************************************************************************
 * @Function: letter_shell_recv_irq_handler
 * @Description: letter-shell接收数据处理
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void letter_shell_recv_irq_handler(UART_HandleTypeDef *huart) {
    shellHandler(&shell, letter_shell_recv_buff);
    HAL_UART_Receive_IT(huart, &letter_shell_recv_buff, 1);
}
