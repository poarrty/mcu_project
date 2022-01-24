/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 19:08:19
 * @Description: TIM中断回调接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_TIM_H__
#define __DRV_TIM_H__

#include "tim.h"

void drv_tim_init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
