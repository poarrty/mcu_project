/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_exti.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-06-21 09:22:36
 * @Description: 外部中断文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_exti.h"
#include "drv_gpio.h"
#include "fal_power.h"
#include "main.h"

/******************************************************************************
 * @Function: HAL_GPIO_EXTI_Callback
 * @Description: 外部中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint16_t} GPIO_Pin
 *******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_4) {
        HAL_Delay(10);

        if (POWER_KEY_READ()) {
            fal_sys_power_on();
        }
    }
}
