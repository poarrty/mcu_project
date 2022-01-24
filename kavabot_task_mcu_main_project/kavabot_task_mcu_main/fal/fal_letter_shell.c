/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_letter_shell.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-06-04 09:27:46
 * @Description: letter_shell指令定义文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_letter_shell.h"
#include "shell_port.h"
#include "fal_power.h"
#include "fal_version.h"
#include "drv_gpio.h"
#include "drv_rtc.h"
#include "bsp_temp.h"
#include "log.h"
#include "sys.h"
#include <string.h>
#include <stdlib.h>

/******************************************************************************
 * @Function: fal_letter_shell_init
 * @Description: letter-shell初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_letter_shell_init(void) {
    userShellInit();
}

/******************************************************************************
 * @Function: get_sys_info
 * @Description: 获取系统相关信息
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_sys_info(int argc, char *argv[]) {
    sys_info_display();
    fal_version_info_display();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_sys_info, get_sys_info, get system info);

/******************************************************************************
 * @Function: debug_usart_choose
 * @Description: 调试串口选择
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int debug_usart_choose(int argc, char *argv[]) {
    if (argc != 2) {
        LOG_CUSTOM(
            "\033[36mUsage:debug_usart_choose <RK3308> <RK1808_3> <RK1808_1> "
            "<RK1808_2>\r\n");
        return 0;
    }

    if (!strcmp(argv[1], "RK3308")) {
        drv_gpio_debug_usart_sw_ctrl(DEBUG_USART_EN_RK3308);
    } else if (!strcmp(argv[1], "RK1808_3")) {
        drv_gpio_debug_usart_sw_ctrl(DEBUG_USART_EN_RK1808_3);
    } else if (!strcmp(argv[1], "RK1808_1")) {
        drv_gpio_debug_usart_sw_ctrl(DEBUG_USART_EN_RK1808_1);
    } else if (!strcmp(argv[1], "RK1808_2")) {
        drv_gpio_debug_usart_sw_ctrl(DEBUG_USART_EN_RK1808_2);
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 debug_usart_choose, debug_usart_choose, debug usart choose);

/******************************************************************************
 * @Function: set_log_level
 * @Description: 设置LOG打印级别
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int set_log_level(int argc, char *argv[]) {
    if (argc != 2) {
        LOG_CUSTOM(
            "\033[36mUsage:set_log_level <OFF> <ERROR> <WARNING> <DEBUG> "
            "<DEBUG_ARRAY> <INFO>\r\n");
        return 0;
    }

    if (!strcmp(argv[1], "OFF")) {
        log_level = LOG_LEVEL_OFF;
    } else if (!strcmp(argv[1], "ERROR")) {
        log_level = LOG_LEVEL_ERROR;
    } else if (!strcmp(argv[1], "WARNING")) {
        log_level = LOG_LEVEL_WARNING;
    } else if (!strcmp(argv[1], "DEBUG")) {
        log_level = LOG_LEVEL_DEBUG;
    } else if (!strcmp(argv[1], "DEBUG_ARRAY")) {
        log_level = LOG_LEVEL_DEBUG_ARRAY;
    } else if (!strcmp(argv[1], "INFO")) {
        log_level = LOG_LEVEL_INFO;
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 set_log_level, set_log_level, set log level);

/******************************************************************************
 * @Function: power_ctrl
 * @Description: 电源控制接口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int power_ctrl(int argc, char *argv[]) {
    if (argc != 2) {
        LOG_CUSTOM("\033[36mUsage:power_ctrl <OFF> <ON> <DISPLAY>\r\n");
        return 0;
    }

    if (!strcmp(argv[1], "OFF")) {
        fal_sys_power_off();
    } else if (!strcmp(argv[1], "ON")) {
        fal_sys_power_on();
    } else if (!strcmp(argv[1], "DISPLAY")) {
        LOG_CUSTOM(
            "\033[36mReturn：xg_cam_pwr_gpio_status:%d(1 is vaild)\033[0m\r\n",
            XC_CAM_PWR_READ());
        LOG_CUSTOM(
            "\033[36mReturn：yt_cam_pwr_gpio_status:%d(1 is vaild)\033[0m\r\n",
            YT_CAM_PWR_READ());
        LOG_CUSTOM(
            "\033[36mReturn：uitrasonic_pwr_gpio_status:%d(1 is "
            "vaild)\033[0m\r\n",
            UITRASONIC_PWR_READ());
        LOG_CUSTOM(
            "\033[36mReturn：sys_pwr_gpio_status:%d(1 is vaild)\033[0m\r\n",
            SYS_PWR_READ());
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 power_ctrl, power_ctrl, power ctrl);

/******************************************************************************
 * @Function: set_rtc_data
 * @Description: 设置RTC数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int set_rtc_data(int argc, char *argv[]) {
    int ret = -1;
    rtc_time_stu_t tm;

    if (argc != 8) {
        LOG_CUSTOM(
            "\033[36mUsage:set_rtc_data <year> <mon> <day> <week> <hour> <min> "
            "<sec>\r\n");
        return ret;
    }

    tm.year = atoi(argv[1]);
    tm.mon = atoi(argv[2]);
    tm.day = atoi(argv[3]);
    tm.week = atoi(argv[4]);
    tm.hour = atoi(argv[5]);
    tm.min = atoi(argv[6]);
    tm.sec = atoi(argv[7]);

    ret = drv_rtc_set_time(tm);

    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 set_rtc_data, set_rtc_data, set rtc data);

/******************************************************************************
 * @Function: get_rtc_data
 * @Description: 获取RTC数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_rtc_data(int argc, char *argv[]) {
    int ret = -1;
    rtc_time_stu_t tm;

    ret = drv_rtc_get_time(&tm);

    LOG_CUSTOM("\033[36mReturn val:%d-%d-%d %d %d:%d:%d\r\n", tm.year, tm.mon,
               tm.day, tm.week, tm.hour, tm.min, tm.sec);

    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 get_rtc_data, get_rtc_data, get rtc data);

/******************************************************************************
 * @Function: get_temp_data
 * @Description: 获取温度数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_temp_data(int argc, char *argv[]) {
    temp_info_stu_t info;

    bsp_get_temp_val(&info);

    LOG_CUSTOM("\033[36mReturn：temp:%lf, adc:%d, vol:%lf, Rt:%lf\033[0m\r\n",
               info.T1, info.adc_val, info.vol_val, info.Rt);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_temp_data, get_temp_data, get temp data);

/******************************************************************************
 * @Function: get_sys_tick
 * @Description: 获取系统tick计数值
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_sys_tick(int argc, char *argv[]) {
    LOG_CUSTOM("\033[36mReturn：current tick:%ld\033[0m\r\n", HAL_GetTick());

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_sys_tick, get_sys_tick, get sys tick);

/******************************************************************************
 * @Function: get_sys_rcc_info
 * @Description: 获取系统启动信息
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_sys_rcc_info(int argc, char *argv[]) {
    uint8_t i = 0;

    for (i = 0; i < 6; i++) {
        if (sys_rcc_flag_info.flag & (1 << i)) {
            LOG_CUSTOM("\033[36m%s\033[0m\r\n", sys_rcc_flag_info.info[i]);
        }
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_sys_rcc_info, get_sys_rcc_info, get sys rcc info);

/******************************************************************************
 * @Function: reboot
 * @Description: 软重启
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *agrv
 *******************************************************************************/
int reboot(int argc, char *agrv[]) {
    //关闭总中断
    __set_FAULTMASK(1);
    //请求单片机重启
    NVIC_SystemReset();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 reboot, reboot, sys reboot);