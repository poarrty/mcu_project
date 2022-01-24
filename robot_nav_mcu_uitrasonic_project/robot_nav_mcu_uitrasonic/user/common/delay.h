/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: delay.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 16:23:53
 * @Description: 系统延时相关接口定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>

void delay_us(uint16_t udelay);
void delay_ms(uint16_t mdelay);
#endif
