/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: delay.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 16:23:53
 * @Description: 系统延时接口定义文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 * 2021-5-11 14:40    guchunqi    V1.1         延时接口从标准库切换到HAL库
 *******************************************************************************/

#include "tim.h"
#include "delay.h"
#include "log.h"

/******************************************************************************
 * @Function: delay_us
 * @Description: 延时udelay个微秒
 * @Input: udelay：延时的微秒数（0~65535）
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint16_t} udelay
 *******************************************************************************/
void delay_us(uint16_t udelay) {
    Error_Check_Callbake(HAL_TIM_Base_Start(&htim6));
    __HAL_TIM_SetCounter(&htim6, 0);
    while (__HAL_TIM_GetCounter(&htim6) < udelay)
        ;
    Error_Check_Callbake(HAL_TIM_Base_Stop(&htim6));
}

void delay_ms(uint16_t mdelay) {
    int i = 0;
    for (i = 0; i < 1000; i++) { delay_us(mdelay); }
}
