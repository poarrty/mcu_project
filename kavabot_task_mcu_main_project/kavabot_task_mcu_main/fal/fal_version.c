/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_version.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-29 18:10:08
 * @Description: 版本信息文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_version.h"
#include "log.h"
#include <string.h>

/******************************************************************************
 * @Function: fal_version_init
 * @Description: 版本初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_init(void) {
    fal_version_info_display();
}

/******************************************************************************
 * @Function: fal_version_info_display
 * @Description: 版本信息显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_info_display(void) {
    LOG_CUSTOM(INFO_COLOR "\r\n\r\n[VERSION]:%s_%s_%s_%s\r\n\r\n" COLOR_END,
               CURRENT_BRANCH, COMMIT_ID, BUILD_TIME, SOFTWARE_VERSION);
}

/******************************************************************************
 * @Function: read_mcu_version_info
 * @Description: 获取MCU版本信息
 * @Input: register_val：存放读取数据的地址
 *         len：要读取得数据长度
 *         index：起始索引
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *register_val
 * @param {uint8_t} len
 * @param {uint8_t} index
 *******************************************************************************/
void read_mcu_version_info(uint8_t *register_val, uint16_t len,
                           uint16_t index) {
    uint8_t buff[40] = {0};

    sprintf((char *) buff, "%s_%s_%s", COMMIT_ID, BUILD_TIME, SOFTWARE_VERSION);

    while (len--) { *register_val++ = buff[index++]; }
}
