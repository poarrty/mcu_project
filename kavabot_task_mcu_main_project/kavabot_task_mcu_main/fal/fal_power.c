/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_power.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-06 09:58:41
 * @Description: 电源管理相关接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_power.h"
#include "drv_gpio.h"
#include "pal_usart3.h"
#include "common_def.h"
#include "log.h"
#include <string.h>

power_var_stu_t power_var;

/******************************************************************************
 * @Function: fal_power_init
 * @Description: 电源管理初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_power_init(void) {
    memset(&power_var, 0, sizeof(power_var_stu_t));

#if __AUTO_POWER_ON_EN__
    fal_sys_power_on();
#else
    fal_sys_power_off();
#endif
}

/******************************************************************************
 * @Function: fal_sys_power_on
 * @Description: 任务板系统上电
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_sys_power_on(void) {
    XG_CAM_PWR_ON();
    YT_CAM_PWR_ON();
    UITRASONIC_PWR_ON();
    SYS_PWR_ON();
    SYS_SET_BIT(power_var.flag, POWER_CURRENT_STATUS_BIT);
    LOG_DEBUG("Sys power on!");
}

/******************************************************************************
 * @Function: fal_sys_power_off
 * @Description: 任务板系统断电
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_sys_power_off(void) {
    XG_CAM_PWR_OFF();
    YT_CAM_PWR_OFF();
    UITRASONIC_PWR_OFF();
    SYS_PWR_OFF();
    SYS_CLR_BIT(power_var.flag, POWER_CURRENT_STATUS_BIT);
    LOG_DEBUG("Sys power off!");
}

/******************************************************************************
 * @Function: read_power_off_request
 * @Description: 获取关机请求
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
void read_power_off_request(uint8_t *register_val, uint16_t len,
                            uint16_t index) {
    uint8_t buff[2] = {0};

    if (SYS_GET_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT)) {
        buff[0] = 0x00;
        buff[1] = POWER_OFF_REQUEST_NEED;
    } else {
        buff[0] = 0x00;
        buff[1] = POWER_OFF_REQUEST_NO_NEED;
    }

    LOG_DEBUG_ARRAY(buff, ARRAY_SIZE(buff));

    while (len--) { *register_val++ = buff[index++]; }
}

/******************************************************************************
 * @Function: write_power_on_or_off_respond
 * @Description:
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
void write_power_on_or_off_respond(uint8_t *register_val, uint16_t len,
                                   uint16_t index) {
    uint8_t buff[2] = {0};

    while (len--) { buff[index++] = *register_val++; }

    if (buff[1] == POWER_OFF_SUCCESS) {
        LOG_DEBUG("Recved sys power off respond!");
        SYS_CLR_BIT(power_var.flag, POWER_OFF_REQUEST_EN_BIT);
        //留给RK3399 shutdown的时间
        HAL_Delay(5000);
        fal_sys_power_off();
        pal_usart3_send_power_manager_respond_msg(
            PAL_USART3_CMD_TYPE_POWER_OFF,
            PAL_USART3_POWER_MANAGER_RESPOND_TYPE_NORMAL);
    } else if (buff[1] == POWER_ON_SUCCESS) {
        LOG_DEBUG("Recved sys power on respond!");
        SYS_CLR_BIT(power_var.flag, POWER_ON_REQUEST_EN_BIT);
        pal_usart3_send_power_manager_respond_msg(
            PAL_USART3_CMD_TYPE_POWER_ON,
            PAL_USART3_POWER_MANAGER_RESPOND_TYPE_NORMAL);
    }
}
