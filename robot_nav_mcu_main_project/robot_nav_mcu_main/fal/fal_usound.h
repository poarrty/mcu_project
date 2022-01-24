/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-7
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

#ifndef _FAL_USOUND_H
#define _FAL_USOUND_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "stdint.h"
#include "lwrb.h"
#include "cmsis_os.h"
#include "usart.h"
/**
 * @ingroup FAL_USOUND
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
#define NAV_CMD_BIT    0x01
#define NAV_FINISH_BIT 0x02
#ifdef ROBOT_SECURITY
#define TASK_FINISH_BIT 0x04
#elif defined BINGO
#define TASK_FINISH_BIT 0x02
#endif

#define USOUND_USART_H huart4
#define USOUND_USART_I UART4

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

} NVG_USOUND_DATA_T, *NVG_USOUND_DATA_P;

#pragma pack(pop)

/*****************************************************************
 * 全局变量声明
 ******************************************************************/
extern char     usound_version[10];
extern uint32_t fileoffest;
extern uint8_t  datalen;
extern uint8_t  is_transdata;

extern uint8_t         usound_flag;
extern uint8_t         usound_recv_byte;
extern lwrb_t          usound_rbuff;
extern osSemaphoreId_t usound_empty_id;
extern osSemaphoreId_t usound_filled_id;
extern osMutexId_t     mutex_usound_data;
/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/****************************************************
功能  : 初始化 fal_usound
返回值: 等于0表示成功，其它值表示失败原因
 ***************************************************/
int fal_usound_init(void);

/****************************************************
功能  ：释放 fal_usound
返回值：等于0表示成功，其它值表示失败原因
***************************************************/
int fal_usound_deInit(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} FAL_USOUND */

#endif
