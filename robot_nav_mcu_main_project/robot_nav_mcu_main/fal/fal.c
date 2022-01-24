/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal.h"
#include "cmsis_os.h"
#include "sys_paras.h"
#include "bsp_led.h"
#include "fal_shell.h"
#include "fal_log.h"
#include "bsp_pmu.h"
#include "bsp_motor.h"
#include "sys_exc.h"
#include "stdio.h"
#include "shell.h"
#include "shell_port.h"
#include "delay.h"
#include "cm_backtrace.h"
#include "cmb_def.h"
#include "wwdg.h"
#include "usart.h"
#include "stdio.h"
#include "fal_pmu.h"
#include "rtc.h"
#include "pal_uros.h"
#include "fal_fdb.h"
#include "elog.h"

/***************************/

/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_FAL 功能适配层 - FAL
 *
 * @brief FAL层实现各种通用的功能模块，并提供统一的接口，为上层调用；
 *        为了便于处理不同的协议，降低协议在上下两层相互转换的复杂和提高系统性能，
 *        该层还支持协议透传功能，即允许上层直接将协议数据发送下来，然后指定到特定的协议模块处理\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
UART_HandleTypeDef *printf_uart        = NULL;
uint32_t            uros_comm_check_ts = 0;
/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern void MX_WWDG_Init(void);

extern int agent_conn_ok;

extern bool is_uros_init_successful;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
int set_local_rtc(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/
void pal_uros_create(void);
void pal_uros_delete(void);
void pal_uros_restart(void);

/*****************************************************************/
/**
 * Function:       fal_Init
 * Description:    初始化FAL层
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_Init(void) {
    // system_init();

#ifndef IAP_UPDATE
    // MX_WWDG_Init();
#endif

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_deinit
 * Description:    释放FAL层资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_deInit(void) {
    return 0;
}

void task_default_run(void *argument) {
    fal_Init();

    /* microros 通信初始化： 指定内存分配接口；通信接口;等等 */
    if (pal_uros_init() != 0) {
        while (1) {
            log_e("pal_uros init error, pal uros stop.");

            osDelay(1000);
        }
    }

    /* 创建 microros 通信任务 */
    pal_uros_create();

    for (;;) {
        /* microros 连接检查 */
        if (is_uros_init_successful == true && osKernelGetTickCount() >= uros_comm_check_ts) {
            /* microros 连接完成后，若速度指令超时，重启任务 */
            if (sys_exc_get(EXC35_NVG_SOC_COM_TO)) {
                log_e("RK3399 speed cmd timeout, restart pal_uros thread.");
                pal_uros_restart();
            }

            uros_comm_check_ts = osKernelGetTickCount() + 3 * 1000;
        }

        osDelay(100);
    }
}

extern uint8_t debug_uart_choose(char *str);
extern int     bsp_led_init(void);

void system_init(void) {
    printf_uart = &huart3;

    fal_log_init();

    fal_shell_init();
    bsp_led_init();

    debug_uart_choose("RK3399");

    /* CmBacktrace initialize */
    cm_backtrace_init("robot_nav_mcu_main", MODEL_NAME, SOFTWARE_VERSION);
    fal_fdb_init();
    sys_paras_init();
    sys_exc_init();
}

osThreadId_t pal_uros_id = NULL;

void pal_uros_create(void) {
    osThreadAttr_t attributes;
    memset(&attributes, 0x0, sizeof(osThreadAttr_t));
    attributes.name       = "task_pal_uros";
    attributes.stack_size = 4 * 1024;
    attributes.priority   = (osPriority_t) osPriorityNormal;

    if (pal_uros_id == NULL) {
        pal_uros_id = osThreadNew(pal_uros_run, NULL, &attributes);

        if (pal_uros_id != NULL) {
            log_i("pal_uros create ok.");
        } else {
            log_e("pal_uros create error, unknow.");
        }
    } else {
        log_e("pal_uros create error, already created.");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pal_uros_create, pal_uros_create, pal_uros_create);

void pal_uros_delete(void) {
    if (pal_uros_id != NULL) {
        if (osThreadTerminate(pal_uros_id) == osOK) {
            pal_uros_id = NULL;

            log_i("pal_uros detete ok.");

        } else {
            log_e("pal_uros detete error, unknow reason");
        }

    } else {
        log_e("pal_uros detete error, thread not created");
    }
}

void pal_uros_restart(void) {
    if (pal_uros_id != NULL) {
        while (!pal_uros_stop()) {
            log_w("wait uros_func_finish");
            osDelay(1000);
        }

        /* 先将 microros 任务挂起 */
        osThreadSuspend(pal_uros_id);
        /* 释放 microros 相关资源 */
        pal_uros_destroy_entities();
        /* 删除 microros 任务 */
        pal_uros_delete();

        /* 启动 microros 任务 */
        pal_uros_create();
    }
}

int __io_putchar(int ch) {
    uint8_t c[1];
    c[0] = ch & 0x00FF;

    if (printf_uart != NULL) {
        HAL_UART_Transmit(printf_uart, &c[0], 1, 10);
    }

    return ch;
}

int _write(int file, char *ptr, int len) {
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }

    return len;
}

int set_local_rtc(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec) {
    RTC_TimeTypeDef rtc_time = {0};
    RTC_DateTypeDef rtc_date = {0};

    rtc_date.Year  = year - 2000;
    rtc_date.Month = moon;
    rtc_date.Date  = date;

    rtc_time.Hours      = hour;
    rtc_time.Minutes    = min;
    rtc_time.Seconds    = sec;
    rtc_time.SubSeconds = 1.0 * msec / 1000 * 255;

    log_i("Set_local_rtc: %d-%d-%d %02d:%02d:%02d.%03d", rtc_date.Year + 2000, rtc_date.Month, rtc_date.Date, rtc_time.Hours, rtc_time.Minutes,
          rtc_time.Seconds, msec);

    HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_407 */
