/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_usart.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:29:28
 * @Description: USART底层驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_usart.h"
#include "shell_port.h"
#include "pal_ir_38k.h"
#include "task_ir_38k.h"
#include "log.h"
#include "crc16.h"
#include "cmsis_os.h"
#include "task_shell.h"
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>

usart_info_stu_t usart3;
usart_info_stu_t usart4;

extern osSemaphoreId_t microros_tx_empty_id;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    // if (huart->Instance == USART2)
    // {
    // 	osSemaphoreRelease(microros_tx_empty_id);
    // }

    if (huart->Instance == USART3 && microros_tx_empty_id) {
        osSemaphoreRelease(microros_tx_empty_id);
    }
}

void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda) {
    osSemaphoreRelease(microros_tx_empty_id);
}

/******************************************************************************
 * @Function: fputc
 * @Description: 重定义printf需要
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
#if __LOG_EN__
int _write(int file, char *data, int len) {
    HAL_StatusTypeDef status;

    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    status = HAL_UART_Transmit(&huart1, (uint8_t *) data, len, 1000);

    return ((status == HAL_OK) ? len : 0);
    ;
}
#endif

/******************************************************************************
 * @Function: drv_usart1_send_buffer
 * @Description: USART1发送指定长度的数据
 * @Input: buf：要发送的数据地址
 *         len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *buf
 * @param {uint16_t} len
 *******************************************************************************/
void drv_usart1_send_buffer(uint8_t *buf, uint16_t len) {
    Error_Check_Callbake(HAL_UART_Transmit(&huart1, buf, len, 1000));
}

/******************************************************************************
 * @Function: drv_usart1_rxne_irq_callbake
 * @Description: USART1接收中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart1_rxne_irq_callbake(UART_HandleTypeDef *huart) {
    if (shell_empty_id != NULL && shell_filled_id != NULL) {
        if (osSemaphoreAcquire(shell_empty_id, 0) == osOK) {
            // produce data
            lwrb_write(&shell_rbuff, &shell_recv_byte, 1);
        }

        osSemaphoreRelease(shell_filled_id);
    }

    HAL_UART_Receive_IT(huart, (uint8_t *) &shell_recv_byte, 1);
}

/******************************************************************************
 * @Function: HAL_UART_RxCpltCallback
 * @Description: 串口接收中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        drv_usart1_rxne_irq_callbake(huart);
    }
}
