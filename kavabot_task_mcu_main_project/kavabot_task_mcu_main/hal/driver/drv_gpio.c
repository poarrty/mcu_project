/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_gpio.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-24 16:45:45
 * @Description: GPIO驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_gpio.h"
#include "log.h"
#include "common_def.h"

/******************************************************************************
 * @Function: drv_gpio_init
 * @Description: GPIO初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_gpio_init(void) {
    drv_gpio_debug_usart_sw_ctrl(__DEBUG_USART_EN_INDEX__);
}

/******************************************************************************
 * @Function: drv_gpio_debug_usart_sw_ctrl
 * @Description: 调试串口切换开关控制
 * @Input: index：串口索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {debug_usart_index_enum_t} index
 *******************************************************************************/
void drv_gpio_debug_usart_sw_ctrl(debug_usart_index_enum_t index) {
    switch (index) {
        case DEBUG_USART_EN_RK3308:
            DEBUG_USART_SW1_L();
            DEBUG_USART_SW2_L();
            break;

        case DEBUG_USART_EN_RK1808_3:
            DEBUG_USART_SW1_H();
            DEBUG_USART_SW2_L();
            break;

        case DEBUG_USART_EN_RK1808_1:
            DEBUG_USART_SW1_L();
            DEBUG_USART_SW2_H();
            break;

        case DEBUG_USART_EN_RK1808_2:
            DEBUG_USART_SW1_H();
            DEBUG_USART_SW2_H();
            break;

        default:
            LOG_ERROR("Unsupport opt!");
            DEBUG_USART_SW1_L();
            DEBUG_USART_SW2_H();
            break;
    }
}
