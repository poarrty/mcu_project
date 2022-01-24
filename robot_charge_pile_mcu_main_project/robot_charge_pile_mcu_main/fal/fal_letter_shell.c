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
#include "fal_version.h"
#include "shell_port.h"
#include "drv_gpio.h"
#include "bsp_temp.h"
#include "task_ir_38k.h"
#include "log.h"
#include "sys.h"
#include "task_shell.h"
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
    fal_shell_init();
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

    LOG_CUSTOM("\033[36mReturn：temp:%f, adc:%d, vol:%f, Rt:%f\033[0m\r\n",
               info.T1, info.adc_val, info.vol_val, info.Rt);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_temp_data, get_temp_data, get temp data);

/******************************************************************************
 * @Function: get_io_status
 * @Description: 获取温度数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_io_status(int argc, char *argv[]) {
    LOG_CUSTOM(
        "\033[36mReturn：charger_output_gpio_status:%d(1 is vaild)\033[0m\r\n",
        CHARGER_OUTPUT_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：wire_charge_output_gpio_status:%d(0 is "
        "vaild)\033[0m\r\n",
        WIRE_CHARGE_OUTPUT_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：auto_charge_output_gpio_status:%d(0 is "
        "vaild)\033[0m\r\n",
        AUTO_CHARGE_OUTPUT_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：charger_electrode_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        CHARGE_ELECTRODE_STATUS_KEY_READ());
    LOG_CUSTOM(
        "\033[36mReturn：cl_add_water_valve_ctrl_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        ADD_WATER_VALVE_CTRL_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：cl_transition_box_valve_ctrl_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        CL_TRANSITION_BOX_VALVE_CTRL_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：cl_sewage_pump_ctrl_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        CL_SEWAGE_PUMP_CTRL_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：cl_clear_water_pump_ctrl_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        CL_CLEAR_WATER_PUMP_CTRL_STATUS_READ());
    LOG_CUSTOM(
        "\033[36mReturn：cl_cleaner_diaphragm_pump_ctrl_gpio_status:%d(1 is "
        "vaild)\033[0m\r\n",
        CL_CLEANER_DIAPHRAGM_PUMP_CTRL_STATUS_READ());

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) |
                     SHELL_CMD_DISABLE_RETURN,
                 get_io_status, get_io_status, get io status);

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