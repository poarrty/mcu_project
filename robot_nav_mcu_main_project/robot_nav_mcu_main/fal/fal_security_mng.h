/*************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:      fal.h
  ** Author:
  ** Version:
  ** Date:
  ** Description:
  ** Others:
  ** Function List:
  ** History:

  ** <time>   <author>    <version >   <desc>
  **
*************************************************/

#ifndef _FAL_SECURITY_MNG_H
#define _FAL_SECURITY_MNG_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "stdint.h"
#include "lwrb.h"
#include "cmsis_os.h"

/**
 * @ingroup FAL_SECURITY_MNG 功能适配层
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define SECURITY_USART_H huart2
#define SECURITY_USART_I USART2

/*****************************************************************
 * 结构定义
 ******************************************************************/
#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint16_t dis1;
    uint16_t dis2;
    uint16_t dis3;
    uint16_t dis4;
    uint16_t dis5;
    uint16_t dis6;
    uint16_t dis7;
    uint16_t dis8;

} TASK_USOUND_DATA_T, *TASK_USOUND_DATA_P;

#pragma pack(pop)

/*****************************************************************
 * 全局变量声明
 ******************************************************************/
extern uint8_t         security_recv_byte;
extern lwrb_t          security_rbuff;
extern osSemaphoreId_t security_empty_id;
extern osSemaphoreId_t security_filled_id;

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/****************************************************
功能  : 初始化 fal_security_mng
返回值: 等于0表示成功，其它值表示失败原因
 ***************************************************/
int fal_security_mng_init(void);

/****************************************************
功能  ：释放 fal_security_mng 资源
返回值：等于0表示成功，其它值表示失败原因
***************************************************/
int fal_security_mng_deInit(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} FAL_SECURITY_MNG */

#endif
