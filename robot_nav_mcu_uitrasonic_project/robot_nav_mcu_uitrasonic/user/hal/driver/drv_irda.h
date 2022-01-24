#ifndef __DRV_IRDA_H__
#define __DRV_IRDA_H__

#include "usart.h"

//串口缓冲区最大长度
#ifndef USART_BUFF_MAX_SIZE
#define USART_BUFF_MAX_SIZE 64
#endif

typedef struct {
    //接收长度
    uint8_t rx_len;
    //接收数组
    uint8_t rx_buff[USART_BUFF_MAX_SIZE];
} usart_info_stu_t;

void drv_irda_init(void);
void drv_irda_irq_handler_callbake(IRDA_HandleTypeDef *huart);

#endif