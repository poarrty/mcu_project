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

#include <stdint.h>

void drv_usart_init(void);
void drv_usart2_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart4_send_buffer(uint8_t *buf, uint16_t len);

#endif
