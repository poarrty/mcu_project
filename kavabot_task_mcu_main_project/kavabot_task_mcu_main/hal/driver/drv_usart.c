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

#include "drv_usart.h"
#include "drv_gpio.h"
#include "shell_port.h"
#include "common_def.h"
#include "pal_usart3.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/unistd.h>

usart_info_stu_t usart3;

extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);

/******************************************************************************
 * @Function: drv_usart_init
 * @Description: USART底层接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_usart_init(void) {
    memset(&usart3, 0, sizeof(usart_info_stu_t));
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    Error_Check_Callbake(
        HAL_UART_Receive_DMA(&huart3, usart3.rx_buff, USART_BUFF_MAX_SIZE));
    // HAL_UART_Receive_IT(&huart3, &usart3.rx_data, 1);
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
    letter_shell_recv_irq_handler(huart);
}

/******************************************************************************
 * @Function: drv_usart2_rxne_irq_callbake
 * @Description: USART2接收中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart2_rxne_irq_callbake(UART_HandleTypeDef *huart) {
    prvvUARTRxISR();
}

/******************************************************************************
 * @Function: drv_usart2_txe_irq_callbake
 * @Description: USART2发送为空中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart2_txe_irq_callbake(UART_HandleTypeDef *huart) {
    prvvUARTTxReadyISR();
}

/******************************************************************************
 * @Function: drv_usart3_send_buffer
 * @Description: USART3发送指定长度的数据
 * @Input: buf：要发送的数据地址
 *         len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *buf
 * @param {uint16_t} len
 *******************************************************************************/
void drv_usart3_send_buffer(uint8_t *buf, uint16_t len) {
    RS485_USART3_DE_H();
    Error_Check_Callbake(HAL_UART_Transmit(&huart3, buf, len, 1000));
    RS485_USART3_DE_L();
}

/******************************************************************************
 * @Function: drv_usart3_rxne_irq_callbake
 * @Description: USART3接收中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart3_rxne_irq_callbake(UART_HandleTypeDef *huart) {
    static uint8_t i = 0;

    switch (usart3.rx_status) {
        case 0:
            if (usart3.rx_data == 0x2E) {
                usart3.rx_buff[usart3.rx_len++] = usart3.rx_data;
                usart3.rx_status++;
            }
            break;

        case 1:
            usart3.rx_buff[usart3.rx_len++] = usart3.rx_data;
            usart3.rx_status++;
            break;

        case 2:
            usart3.rx_buff[usart3.rx_len++] = usart3.rx_data;
            i++;
            if (i == usart3.rx_buff[1]) {
                i = 0;
                usart3.rx_status++;
            }
            break;

        case 3:
            usart3.rx_buff[usart3.rx_len++] = usart3.rx_data;
            pal_usart3_recv_msg_parse(usart3);
            usart3.rx_len = 0;
            usart3.rx_status = 0;
            break;
    }

    Error_Check_Callbake(HAL_UART_Receive_IT(huart, &usart3.rx_data, 1));
}

/******************************************************************************
 * @Function: drv_usart3_rx_idle_irq_callbake
 * @Description: USART3接收空闲中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart3_rx_idle_irq_callbake(UART_HandleTypeDef *huart) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    Error_Check_Callbake(HAL_UART_DMAStop(huart));
    usart3.rx_len =
        USART_BUFF_MAX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
    pal_usart3_recv_msg_parse(usart3);
    memset(&usart3, 0, sizeof(usart_info_stu_t));
    Error_Check_Callbake(
        HAL_UART_Receive_DMA(huart, usart3.rx_buff, USART_BUFF_MAX_SIZE));
}

/******************************************************************************
 * @Function: drv_usart_irq_handler_callbake
 * @Description: USART中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_usart_irq_handler_callbake(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE) != RESET) {
            drv_usart2_rxne_irq_callbake(huart);
        }

        if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE) != RESET) {
            drv_usart2_txe_irq_callbake(huart);
        }

        HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
    } else if (huart->Instance == USART3) {
        if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) != RESET) {
            drv_usart3_rx_idle_irq_callbake(huart);
        }

        HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
    }
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
    if (huart->Instance == USART1) {
        drv_usart1_rxne_irq_callbake(huart);
    } else if (huart->Instance == USART3) {
        drv_usart3_rxne_irq_callbake(huart);
    }
}
