/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: sys.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-06-03 19:03:21
 * @Description: 系统相关接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "sys.h"
#include "log.h"
#include "main.h"

sys_rcc_flag_info_stu_t sys_rcc_flag_info = {
    0,
    {"Pin reset!", "Power reset!", "Soft reset!", "IWDG reset!", "WWDG reset!",
     "Low power reset!"}};

/******************************************************************************
 * @Function: sys_init
 * @Description: 系统相关接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void sys_init(void) {
    sys_info_display();
    check_rcc_flag();
}

/******************************************************************************
 * @Function: sys_info_display
 * @Description: 系统信息显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void sys_info_display(void) {
    sys_info_stu_t sys_info;

    sys_info.device_id = HAL_GetDEVID();
    sys_info.hal_version = HAL_GetHalVersion();
    sys_info.revision_id = HAL_GetREVID();
    sys_info.uid[0] = HAL_GetUIDw0();
    sys_info.uid[1] = HAL_GetUIDw1();
    sys_info.uid[2] = HAL_GetUIDw2();
    sys_info.sys_clk = HAL_RCC_GetSysClockFreq();
    sys_info.hclk = HAL_RCC_GetHCLKFreq();
    sys_info.pclk1 = HAL_RCC_GetPCLK1Freq();
    sys_info.pclk2 = HAL_RCC_GetPCLK2Freq();

    LOG_CUSTOM(INFO_COLOR
               "\r\n***********************************************************"
               "**********\r\n" COLOR_END);
    LOG_CUSTOM(INFO_COLOR "* device id:   0x%08ld\r\n" COLOR_END,
               sys_info.device_id);
    LOG_CUSTOM(INFO_COLOR "* hal version: 0x%08ld\r\n" COLOR_END,
               sys_info.hal_version);
    LOG_CUSTOM(INFO_COLOR "* revision id: 0x%08ld\r\n" COLOR_END,
               sys_info.revision_id);
    LOG_CUSTOM(INFO_COLOR
               "* uid:         0x%08lx-0x%08lx-0x%08lx\r\n" COLOR_END,
               sys_info.uid[0], sys_info.uid[1], sys_info.uid[2]);
    LOG_CUSTOM(INFO_COLOR "* sys clk:     %08ldHz\r\n" COLOR_END,
               sys_info.sys_clk);
    LOG_CUSTOM(INFO_COLOR "* hclk:        %08ldHz\r\n" COLOR_END,
               sys_info.hclk);
    LOG_CUSTOM(INFO_COLOR "* pclk1:       %08ldHz\r\n" COLOR_END,
               sys_info.pclk1);
    LOG_CUSTOM(INFO_COLOR "* pclk2:       %08ldHz\r\n" COLOR_END,
               sys_info.pclk2);
    LOG_CUSTOM(INFO_COLOR
               "***************************************************************"
               "******\r\n" COLOR_END);
}

/******************************************************************************
 * @Function: check_rcc_flag
 * @Description: 检查复位标识
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void check_rcc_flag(void) {
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        //这是外部RST管脚复位
        sys_rcc_flag_info.flag |= (1 << 0);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
        //这是上电复位
        sys_rcc_flag_info.flag |= (1 << 1);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
        //这是软件复位
        sys_rcc_flag_info.flag |= (1 << 2);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
        //这是独立看门狗复位
        sys_rcc_flag_info.flag |= (1 << 3);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
        //这是窗口看门狗复位
        sys_rcc_flag_info.flag |= (1 << 4);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
        //这是低功耗复位
        sys_rcc_flag_info.flag |= (1 << 5);
    }

    //清除RCC中复位标志
    __HAL_RCC_CLEAR_RESET_FLAGS();
}