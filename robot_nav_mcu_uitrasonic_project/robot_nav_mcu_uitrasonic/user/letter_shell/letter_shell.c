/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: letter_shell.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-06-04 09:27:46
 * @Description: letter_shell指令定义文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "letter_shell.h"
#include "shell_port.h"
#include "drv_gpio.h"
#include "log.h"
#include "sys.h"
#include "version.h"
#include <string.h>
#include "bsp_09.h"

/******************************************************************************
 * @Function: letter_shell_init
 * @Description: letter-shell初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void letter_shell_init(void) {
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
 * @param {char} *agrv
 *******************************************************************************/
int get_sys_info(int argc, char *agrv[]) {
    sys_info_display();
    version_info_display();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_sys_info, get_sys_info, get system info);

/******************************************************************************
 * @Function: set_log_level
 * @Description: 设置LOG打印级别
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *agrv
 *******************************************************************************/
int set_log_level(int argc, char *agrv[]) {
    int ret = -1;

    if (argc != 2) {
        shellWriteString(&shell,
                         "\033[36mUsage:set_log_level <OFF> <ERROR> <WARNING> "
                         "<DEBUG> <DEBUG_ARRAY> <INFO>\r\n");
        return ret;
    }

    if (!strcmp(agrv[1], "OFF")) {
        log_level = LOG_LEVEL_OFF;
        ret = log_level;
    } else if (!strcmp(agrv[1], "ERROR")) {
        log_level = LOG_LEVEL_ERROR;
        ret = log_level;
    } else if (!strcmp(agrv[1], "WARNING")) {
        log_level = LOG_LEVEL_WARNING;
        ret = log_level;
    } else if (!strcmp(agrv[1], "DEBUG")) {
        log_level = LOG_LEVEL_DEBUG;
        ret = log_level;
    } else if (!strcmp(agrv[1], "DEBUG_ARRAY")) {
        log_level = LOG_LEVEL_DEBUG_ARRAY;
        ret = log_level;
    } else if (!strcmp(agrv[1], "INFO")) {
        log_level = LOG_LEVEL_INFO;
        ret = log_level;
    }

    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 set_log_level, set_log_level, set log level);

int set_ultrasonicl_thres(int argc, char *agrv[]) {
    int ret = -1;

    if (argc != 2) {
        shellWriteString(&shell,
                         "\033[36mUsage:set_ultrasonicl_thre  <SMALL>  "
                         "<MIDDLE>  <HIGH>\r\n");
        return ret;
    }

    if (!strcmp(agrv[1], "SMALL")) {
        log_ultrasonicl_thres = LOG_ULTRASONIC_THRES_LOW;
        ret = log_ultrasonicl_thres;
    } else if (!strcmp(agrv[1], "MIDDLE")) {
        log_ultrasonicl_thres = LOG_LEVEL_ERROR;
        ret = log_ultrasonicl_thres;
    } else if (!strcmp(agrv[1], "HIGH")) {
        log_ultrasonicl_thres = LOG_LEVEL_WARNING;
        ret = log_ultrasonicl_thres;
    }
    bsp_09_channel_loop_thres_setup();
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 set_ultrasonicl_thres, set_ultrasonicl_thres,
                 set ultrasonicl thres);

int set_ultrasonicl_state(int argc, char *agrv[]) {
    int ret = -1;

    if (argc != 2) {
        shellWriteString(&shell,
                         "\033[36mUsage:set_ultrasonicl_state <ON> <OFF>\r\n");
        return ret;
    }

    if (!strcmp(agrv[1], "ON")) {
        digital_ultrasonic_state = ELMOS09_ON;
        ret = digital_ultrasonic_state;
    } else if (!strcmp(agrv[1], "OFF")) {
        digital_ultrasonic_state = ELMOS09_OFF;
        ret = digital_ultrasonic_state;
    }

    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 set_ultrasonicl_state, set_ultrasonicl_state,
                 set ultrasonicl state);