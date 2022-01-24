/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: hal.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 14:49:09
 * @Description: 硬件抽象层接口入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_adc.h"
#include "drv_gpio.h"
#include "drv_tim.h"
#include "drv_usart.h"
#include "bsp_e2prom.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"

/******************************************************************************
 * @Function: hal_init
 * @Description: 硬件抽象层初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int hal_init(void) {
    drv_gpio_init();
    drv_adc_init();
    drv_tim_init();
    drv_usart_init();
    bsp_e2prom_init();

    return 0;
}

/******************************************************************************
 * @Function: hal_DeInit
 * @Description: 硬件抽象层反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int hal_DeInit(void) {
    return 0;
}
