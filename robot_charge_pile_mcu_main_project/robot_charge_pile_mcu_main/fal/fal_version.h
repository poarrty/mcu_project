/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_version.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:44:26
 * @Description: 版本信息头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_VERSION_H__
#define __FAL_VERSION_H__

#define BT_POWER_ON \
    HAL_GPIO_WritePin(BT_POWER_EN_GPIO_Port, BT_POWER_EN_Pin, GPIO_PIN_RESET)
#define BT_POWER_OFF \
    HAL_GPIO_WritePin(BT_POWER_EN_GPIO_Port, BT_POWER_EN_Pin, GPIO_PIN_SET)

void fal_version_init(void);
void fal_version_info_display(void);

#endif
