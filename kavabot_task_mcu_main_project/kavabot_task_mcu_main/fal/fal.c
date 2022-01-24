/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 14:48:56
 * @Description: 功能抽象层接口入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_version.h"
#include "fal_letter_shell.h"
#include "fal_power.h"
#include "fal_uitrasonic.h"

/******************************************************************************
 * @Function: fal_init
 * @Description: 功能抽象层初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int fal_init(void) {
    fal_version_init();
    fal_letter_shell_init();
    fal_power_init();
    fal_uitrasonic_init();

    return 0;
}

/******************************************************************************
 * @Function: fal_DeInit
 * @Description: 功能抽象层反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int fal_DeInit(void) {
    return 0;
}
