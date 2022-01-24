/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      fal.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-01-
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-01 robot创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_pmu.h"
#include "cmsis_os.h"
#include "sys_paras.h"
#include "bsp_led.h"
#include "bsp_battery.h"
#include "sys_pubsub.h"
#include "bsp_pmu.h"

#define LOG_TAG "fal_pmu"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_PMU 功能适配层 - FAL
 *
 * @brief 电源管理单元功能\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
static PMU_STA_E pmu_sta        = PMU_STA_STANDBY;
static uint32_t  system_up_ts   = 0;
static uint32_t  system_down_ts = 0;

static uint16_t mod_task_soc_sta = SOC_STA_POWEROFF;

///< Modbus 寄存器
uint16_t mod_nvg_soc_sta = SOC_STA_POWEROFF;
uint16_t mod_nvg_updown  = NVG_SOC_UP_CMD;

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
extern void fal_security_power_on(void);
extern void fal_security_power_off(void);

static void fal_pmu_sta_handle(void);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_pmu_init
 * Description:    初始化 fal_pmu
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
int fal_pmu_init(void) {
    bsp_pmu_all_on();
    bsp_led_on(LED_POWER_BUTTON);
    pmu_sta = PMU_STA_STANDBY;

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_pmu_deInit
 * Description:    释放 fal_pmu 资源
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
int fal_pmu_deInit(void) {
    return 0;
}

void task_fal_power_run(void *argument) {
    uint32_t     sub_evt;
    BUTTON_STA_T button_sta;
    PMU_CTRL_T   pmu_ctrl;
    BATT_STA_T   batt_sta;

    fal_pmu_init();

    for (;;) {
        ///< 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_pmu, SYS_EVT_BUTTON_STA | SYS_EVT_PMU_CTRL | SYS_EVT_BATT_STA, osFlagsWaitAny, 100);

        if (sub_evt & osFlagsError) {
            continue;
        }

        ///< 读取主题数据
        if (sub_evt & SYS_EVT_PMU_CTRL) {
            xQueuePeek(topic_pmu_ctrl, &pmu_ctrl, 100);

            if (pmu_ctrl.cmd == PMU_STA_STARTING && pmu_sta == PMU_STA_STANDBY) {
                pmu_sta = PMU_STA_STARTING;
            } else if (pmu_ctrl.cmd == PMU_STA_SHUTDOWN && pmu_sta == PMU_STA_RUNNING) {
                pmu_sta = PMU_STA_SHUTDOWN;
            }
        }

        if (sub_evt & SYS_EVT_BUTTON_STA) {
            xQueuePeek(topic_button, &button_sta, 100);

            if (button_sta.button == BUTTON_PWR && button_sta.event == SINGLE_CLICK) {
                ///< 在运行状态下有开关机按键触发，则启动关机流程
                if (pmu_sta == PMU_STA_RUNNING) {
                    pmu_sta = PMU_STA_SHUTDOWN;
                }
                ///< 在待机状态下有开关机按键触发，则导航板外设上电
                else if (pmu_sta == PMU_STA_STANDBY) {
                    pmu_sta = PMU_STA_STARTING;
                }
            }
        }

        if (sub_evt & SYS_EVT_BATT_STA) {
            xQueuePeek(topic_batt_sta, &batt_sta, 100);

            if (batt_sta.sta == BATT_LOW_DETECT && pmu_sta == PMU_STA_RUNNING) {
                pmu_sta = PMU_STA_SHUTDOWN;
            }
        }

        fal_pmu_sta_handle();

        ///< 清除处理过程中的按键事件
        osEventFlagsClear(sys_suber_pmu, SYS_EVT_BUTTON_STA);
        ///< 清除处理过程中的开关机命令
        osEventFlagsClear(sys_suber_pmu, SYS_EVT_PMU_CTRL);
    }
}

static void fal_pmu_sta_handle(void) {
    PMU_STA_T pmu_sta_topic_data;

    uint32_t ts_up, ts_down;
    uint8_t  nvg_flag, task_flag;  ///< 打印辅助标志

    switch (pmu_sta) {
        case PMU_STA_STARTING:
            system_up_ts = osKernelGetTickCount();

            bsp_pmu_all_on();

            ///< 任务板开机
            fal_security_power_on();

            mod_nvg_soc_sta        = SOC_STA_STARTING;  ///< NVG SOC 状态复位为开机中
            mod_task_soc_sta       = SOC_STA_STARTING;  ///< TASK SOC 状态复位为开机中
            mod_nvg_updown         = NVG_SOC_UP_CMD;
            pmu_sta_topic_data.sta = PMU_STA_STARTING;
            pub_topic(SYS_EVT_PMU_STA, &pmu_sta_topic_data);

            ///< 记录开始上电事件戳
            ts_up     = osKernelGetTickCount();
            nvg_flag  = 0;
            task_flag = 0;

            osDelay(300);
            log_i("PMU STATE: [staring]");
            while (1) {
                if (mod_nvg_soc_sta == SOC_STA_RUNNING && nvg_flag == 0) {
                    nvg_flag = 1;
                    log_i("NVG SOC start success!");
                }

                if (mod_task_soc_sta == SOC_STA_RUNNING && task_flag == 0) {
                    task_flag = 1;
                    log_i("TASK SOC start success!");
                }

                ///< 收到开机完成反馈
                if (mod_nvg_soc_sta == SOC_STA_RUNNING && mod_task_soc_sta == SOC_STA_RUNNING) {
                    log_i("PMU STATE: [staring] success!");
                    pmu_sta = PMU_STA_RUNNING;
                    break;  ///< 正常退出 PMU_STA_STARTING 状态 while
                } else if (osKernelGetTickCount() > ts_up + sys_paras.nvg_rk_up_timeout_ms) {
                    log_w("PMU STATE: [staring] timeout!");
                    pmu_sta = PMU_STA_RUNNING;
                    break;  ///< 超时退出 PMU_STA_STARTING 状态 while
                }

                fal_security_power_on();
                osDelay(1000);
            }

            log_i("PMU STATE: system up finish.");

            pmu_sta_topic_data.sta = PMU_STA_RUNNING;
            pub_topic(SYS_EVT_PMU_STA, &pmu_sta_topic_data);

        case PMU_STA_RUNNING:
            log_i("PMU STATE: [running]");

            ///< 状态 PMU_STA_RUNNING，退出
            break;

        ///< 关机流程起点
        case PMU_STA_SHUTDOWN:
            log_i("PMU STATE: [shutdown]");

            log_i("PMU STATE: system down begin..");

            system_down_ts = osKernelGetTickCount();

            bsp_led_off(LED_POWER_BUTTON);

            ///< 任务板关机
            fal_security_power_off();

            mod_nvg_soc_sta        = SOC_STA_SHUTDOWN;  ///< NVG SOC 状态复位为关机中
            mod_task_soc_sta       = SOC_STA_SHUTDOWN;  ///< TASK SOC 状态复位为关机中
            mod_nvg_updown         = NVG_SOC_DOWN_CMD;
            pmu_sta_topic_data.sta = PMU_STA_SHUTDOWN;
            pub_topic(SYS_EVT_PMU_STA, &pmu_sta_topic_data);

            ///< 记录开始上电事件戳
            ts_down   = osKernelGetTickCount();
            nvg_flag  = 0;
            task_flag = 0;

            while (1) {
                if (mod_nvg_soc_sta == SOC_STA_STANDBY && nvg_flag == 0) {
                    nvg_flag = 1;
                    log_i("NVG SOC shutdown success!");
                }

                if (mod_task_soc_sta == SOC_STA_STANDBY && task_flag == 0) {
                    task_flag = 1;
                    log_i("TASK SOC shutdown success!");
                }

                ///< 收到关机完成反馈
                if (mod_nvg_soc_sta == SOC_STA_STANDBY && mod_task_soc_sta == SOC_STA_STANDBY) {
                    log_i("PMU STATE: [shutdown ing] success!");
                    pmu_sta = PMU_STA_STANDBY;
                    break;  ///< 正常退出 SHUTDOWN 状态 while，状态为
                            ///< PMU_STA_STANDBY
                } else if (osKernelGetTickCount() > ts_down + sys_paras.nvg_rk_up_timeout_ms) {
                    log_w("PMU STATE: [shutdown ing] timeout!");
                    pmu_sta = PMU_STA_STANDBY;
                    break;  ///< 超时退出 SHUTDOWN 状态 while，状态为
                            ///< PMU_STA_STANDBY
                }

                fal_security_power_off();
                osDelay(1000);
            }

            log_i("PMU STATE: system down finish.");

            ///< 设备断电
            bsp_pmu_all_off();

            pmu_sta_topic_data.sta = PMU_STA_STANDBY;
            pub_topic(SYS_EVT_PMU_STA, &pmu_sta_topic_data);

            // break;          ///< 状态已经设置成 PMU_STA_STANDBY，退出

        case PMU_STA_STANDBY:

            log_i("PMU STATE: [standby]");

            ///< 状态 PMU_STA_STANDBY，退出
            break;

        default:
            break;
    }
}

void fal_pmu_set_sta(PMU_STA_E sta) {
    pmu_sta = sta;
}

PMU_STA_E fal_pmu_get_sta(void) {
    return pmu_sta;
}

void nvg_soc_sta_set(uint8_t sta) {
    mod_nvg_soc_sta = sta;
}

void task_soc_sta_set(uint8_t sta) {
    if (sta) {
        mod_task_soc_sta = SOC_STA_RUNNING;
    } else {
        mod_task_soc_sta = SOC_STA_STANDBY;
    }
}

uint32_t fal_pmu_sys_up_time(void) {
    return osKernelGetTickCount() - system_up_ts;
}

uint32_t fal_pmu_sys_down_time(void) {
    return osKernelGetTickCount() - system_down_ts;
}

#ifdef __cplusplus
}
#endif

/* @} FAL_PMU */
/* @} Robot_FAL */
