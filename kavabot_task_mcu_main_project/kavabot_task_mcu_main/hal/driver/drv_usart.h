/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_usart.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 15:05:21
 * @Description: USART底层驱动接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include "usart.h"
#include <stdint.h>

//串口缓冲区最大长度
#ifndef USART_BUFF_MAX_SIZE
#define USART_BUFF_MAX_SIZE 64
#endif

typedef struct {
    uint8_t rx_data;
    uint8_t rx_status;
    //接收长度
    uint8_t rx_len;
    //接收数组
    uint8_t rx_buff[USART_BUFF_MAX_SIZE];
} usart_info_stu_t;

extern usart_info_stu_t usart3;

void drv_usart_init(void);
void drv_usart1_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart3_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart_irq_handler_callbake(UART_HandleTypeDef *huart);

#endif
