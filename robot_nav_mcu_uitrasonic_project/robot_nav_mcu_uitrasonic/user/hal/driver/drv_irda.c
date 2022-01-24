#include "drv_irda.h"
#include "drv_gpio.h"
#include "log.h"
#include <string.h>

usart_info_stu_t usart3;

uint8_t rx_data;

void drv_irda_init(void) {
    memset(&usart3, 0, sizeof(usart_info_stu_t));
    HAL_IRDA_Receive_DMA(&hirda3, (uint8_t *) &USART1->DR, 1);
    HAL_UART_Receive_DMA(&huart1, (uint8_t *) &USART3->DR, 1);
    // HAL_UART_Transmit_DMA(&huart1, &USART1->DR, 1);
    //__HAL_UART_ENABLE_IT(&hirda3, UART_IT_IDLE);
    //__HAL_UART_CLEAR_IDLEFLAG(&hirda3);
    IRDA_SD_EN_L();
}

/******************************************************************************
 * @Function: drv_irda_rx_idle_irq_callbake
 * @Description: IRDA接收空闲中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {IRDA_HandleTypeDef} *huart
 *******************************************************************************/
void drv_irda_rx_idle_irq_callbake(IRDA_HandleTypeDef *huart) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    Error_Check_Callbake(HAL_IRDA_DMAStop(huart));
    usart3.rx_len =
        USART_BUFF_MAX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

    // LOG_DEBUG_ARRAY(usart3.rx_buff, usart3.rx_len);
    HAL_IRDA_Transmit(huart, usart3.rx_buff, usart3.rx_len, 1000);

    memset(&usart3, 0, sizeof(usart_info_stu_t));
    Error_Check_Callbake(
        HAL_IRDA_Receive_DMA(huart, usart3.rx_buff, USART_BUFF_MAX_SIZE));
}

/******************************************************************************
 * @Function: drv_irda_irq_handler_callbake
 * @Description: IRDA中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {IRDA_HandleTypeDef} *huart
 *******************************************************************************/
void drv_irda_irq_handler_callbake(IRDA_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) != RESET) {
            drv_irda_rx_idle_irq_callbake(huart);
        }

        HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
    }
}