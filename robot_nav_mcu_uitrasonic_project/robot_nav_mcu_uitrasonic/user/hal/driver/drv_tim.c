/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 19:08:39
 * @Description: TIM中断回调接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_tim.h"
#include "common_def.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_tim_init
 * @Description: TIM底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim_init(void) {
    Error_Check_Callbake(HAL_TIM_Base_Start_IT(&htim1));
}

/******************************************************************************
 * @Function: drv_tim1_update_irq_callbake
 * @Description: TIM1更新中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_tim1_update_irq_callbake(void) {}

/******************************************************************************
 * @Function: HAL_TIM_PeriodElapsedCallback
 * @Description: TIM更新中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        drv_tim1_update_irq_callbake();
    }
}
