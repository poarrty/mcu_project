/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_usart.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 15:05:41
 * @Description: USART底层驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "SEGGER_RTT.h"
#include "usart.h"
#include "shell_port.h"
#include "drv_usart.h"
#include "drv_gpio.h"
#include "common_def.h"
#include "log.h"
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>
#include <ota.h>
/******************************************************************************
 * @Function: drv_usart_init
 * @Description: 串口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_usart_init(void) {
    // TODO
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

    status = HAL_UART_Transmit(&huart2, (uint8_t *) data, len, 1000);

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
void drv_usart2_send_buffer(uint8_t *buf, uint16_t len) {
    Error_Check_Callbake(HAL_UART_Transmit(&huart2, buf, len, 1000));
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
void drv_usart2_rxne_irq_callbake(UART_HandleTypeDef *huart) {
    letter_shell_recv_irq_handler(huart);
}

/******************************************************************************
 * @Function: drv_usart4_send_buffer
 * @Description: USART4发送指定长度的数据
 * @Input: buf：要发送的数据地址
 *         len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *buf
 * @param {uint16_t} len
 *******************************************************************************/
void drv_usart4_send_buffer(uint8_t *buf, uint16_t len) {
    RS485_USART4_DE_H();
    Error_Check_Callbake(HAL_UART_Transmit(&huart4, buf, len, 1000));
    RS485_USART4_DE_L();
}

/******************************************************************************
 * @Function: HAL_UART_RxCpltCallback
 * @Description: USART接收中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART4) {
        ota_recv(huart);
    }
}
