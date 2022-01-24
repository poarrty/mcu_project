/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: app.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 14:47:48
 * @Description: APP入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "hal.h"
#include "common.h"
#include "pal.h"
#include "letter_shell.h"

/******************************************************************************
 * @Function: app_init
 * @Description: APP初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int app_init(void) {
    hal_init();
    common_init();
    pal_init();
    letter_shell_init();

    return 0;
}

/******************************************************************************
 * @Function: app_DeInit
 * @Description: APP反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int app_DeInit(void) {
    return 0;
}
