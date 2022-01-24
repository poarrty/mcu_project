/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_uitrasonic.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-10-08 19:46:14
 * @Description: 超声业务层源文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_uitrasonic.h"
#include "bsp_s09.h"
#include "common_def.h"

/******************************************************************************
 * @Function: fal_uitrasonic_init
 * @Description: 超声初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_uitrasonic_init(void) {
    bsp_s09_init();
}

/******************************************************************************
 * @Function: fal_uitrasonic_server
 * @Description: 超声服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_uitrasonic_server(void) {
    if (SYS_GET_BIT(s09_var.flag, UITRASONIC_DETECT_EN_BIT)) {
        bsp_s09_distance_detect_server();
    }
}