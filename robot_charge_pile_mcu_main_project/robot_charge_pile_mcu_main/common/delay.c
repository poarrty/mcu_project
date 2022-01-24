/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: delay.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:15:35
 * @Description: 系统延时接口定义文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "delay.h"
#include "log.h"

/******************************************************************************
 * @Function: delay_us
 * @Description: 延时udelay个微秒
 * @Input: udelay：延时的微秒数（0~65535）
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {uint16_t} udelay
 *******************************************************************************/
HAL_StatusTypeDef delay_us(uint16_t udelay) {
    HAL_StatusTypeDef status;

    status = HAL_TIM_Base_Start(&htim7);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    __HAL_TIM_SetCounter(&htim7, 0);

    while (__HAL_TIM_GetCounter(&htim7) < udelay)
        ;

    status = HAL_TIM_Base_Stop(&htim7);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}
