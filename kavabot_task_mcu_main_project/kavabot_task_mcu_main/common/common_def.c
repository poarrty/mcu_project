/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: common_def.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 17:38:26
 * @Description: 公共定义文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "common_def.h"
#include <string.h>

#if __LOG_EN__
log_level_enum_t log_level = LOG_LEVEL_WARNING;
#endif

global_var_stu_t global_var;

/******************************************************************************
 * @Function: common_def_init
 * @Description: 公共定义初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void common_def_init(void) {
    // TODO
}
