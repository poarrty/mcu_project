/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-29
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_misc.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "sys_exc.h"
#include "bsp_ntc.h"
#include "bsp_tough_edge.h"
#include "fal_pmu.h"
#include "wwdg.h"
#include "tim.h"
#include "rtc.h"
#include "shell.h"
#include "shell_port.h"
#include "sys_paras.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "bsp_led.h"
#include "pal_uros.h"
#include "lwrb.h"
#include "cJSON.h"
#include "sensor_msgs/msg/range.h"
#include "FreeRTOS.h"
#include "std_msgs/msg/string.h"
#include "sys_exc.h"
#include "std_msgs/msg/bool.h"
#include "std_msgs/msg/empty.h"
#include "std_msgs/msg/float32.h"
#include "flashdb.h"
#include "chassis_interfaces/msg/run_task_condition.h"

#define LOG_TAG "fal_misc"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_MISC 功能适配层 - 杂项(急停\水浸\雨水\触边)\ 内外部温湿度\
 * 触边传感器 \ SOC 连接状态\ 系统运行状态\ 窗口看门狗喂狗\ coredown 信息保存\
 * 自检状态
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define NVG_SOC_LIFE_EXTEND 500

#define IO_SHAKE_CNT_MAX 3

#define TEMP_PUB_BIT 1
/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
osMutexId_t mutex_touch_edge_data_upload = NULL;

const osMutexAttr_t mutex_touch_edge_data_upload_attr = {
    "mutex_touch_edge_data_upload",         // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

ROBOT_THTB_T       robot_thtb       = {0.0, 0.0, 0.0, 0.0};
ROBOT_TOUGH_EDGE_T robot_tough_edge = {0, 0.0, 0, 0.0};
SELFTEST_STA_T     selftest_sta     = {0};

static publisher               g_uros_infrared[2] = {0};
static sensor_msgs__msg__Range uros_infrared[2];

static publisher           g_emerg = {0};
static std_msgs__msg__Bool emerg_sta;

static publisher           g_crash = {0};
static std_msgs__msg__Bool crash_sta;

static publisher              g_temperature = {0};
static std_msgs__msg__Float32 temperature;

///< sub temperature_update
static subscrption         g_temperature_update;
static std_msgs__msg__Bool temperature_update;
///< sub emerg_update
static subscrption         g_emerg_update;
static std_msgs__msg__Bool emerg_update;
///< sub crash_update
static subscrption         g_crash_update;
static std_msgs__msg__Bool crash_update;

static publisher                                 g_runTaskCondition = {0};
static chassis_interfaces__msg__RunTaskCondition runTaskCondition;
static char                                      runTaskCondition_type[20];

static subscrption          g_runTaskCondition_update;
static std_msgs__msg__Empty runTaskCondition_update;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static uint32_t    nvg_soc_life_ts = 0;
static osTimerId_t auto_on_timer   = NULL;

SFU_RESET_IdTypeDef e_wakeup_source_id = SFU_RESET_UNKNOWN;

osSemaphoreId_t infrared_empty_id   = NULL;
osSemaphoreId_t infrared_filled_id  = NULL;
osMutexId_t     mutex_infrared_data = NULL;

const osMutexAttr_t mutex_infrared_data_attr = {
    "mutex_infrared_data",                  // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

uint32_t             task_misc_publicBuffer[512];
StaticTask_t         task_misc_publicControlBlock;
const osThreadAttr_t task_misc_public_attributes = {
    .name       = "task_misc_public",
    .cb_mem     = &task_misc_publicControlBlock,
    .cb_size    = sizeof(task_misc_publicControlBlock),
    .stack_mem  = &task_misc_publicBuffer[0],
    .stack_size = sizeof(task_misc_publicBuffer),
    .priority   = (osPriority_t) osPriorityNormal,
};

/* 急停状态发布标志 */
static uint8_t emerg_pub_flag = 0;
/* 碰撞状态发布标志 */
static uint8_t crash_pub_flag = 0;
/* 碰撞状态发布结束时间 */
static uint32_t crash_pub_end_ts = 0;

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern TIM_HandleTypeDef htim6;
extern uint8_t           motor_speed_vw_set(int16_t speed_v, int16_t speed_w);

extern struct fdb_kvdb kvdb;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void SFU_BOOT_ManageResetSources(void);
static void task_fal_misc_public_run(void *argument);
static void temperature_update_subscription_callback(const void *msgin);
static void emerg_update_subscription_callback(const void *msgin);
static void crash_update_subscription_callback(const void *msgin);

/*****************************************************************
 * 函数定义
 ******************************************************************/
extern uint8_t soft_button_click(void);

extern void *pvPortMalloc(size_t xWantedSize);
extern void  vPortFree(void *pv);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/
static void auto_on_cb(void *argument);
static void selftest_sta_update(void);

static void fal_infrared_data_public(float data, sensor_msgs__msg__Range *msg_uros_infrared, publisher pub_uros_infrared);

cJSON_Hooks cjson_hooks;

static char *create_exc_json_string(void);

static void fal_sys_exc_public(char *json_string);

static publisher                                g_sys_exc;
static uint8_t                                  temperature_flag = 0;
static std_msgs__msg__String                    sys_exc;
__attribute__((section(".ccmram"))) static char sys_exc_buffer[1024];

/*****************************************************************/
/**
 * Function:       fal_misc_init
 * Description:    初始化 fal_misc
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
int fal_misc_init(void) {
    char *infrared_name[2] = {"/ir_edge_2", "/ir_edge_1"};

    for (int num = 0; num < 2; num++) {
        publisher_init(&g_uros_infrared[num], ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range), infrared_name[num], &uros_infrared[num],
                       BEST, OVERWRITE, sizeof(sensor_msgs__msg__Range));
    }

    publisher_init(&g_sys_exc, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/device/status", &sys_exc, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__String));

    publisher_init(&g_emerg, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/emerg", &emerg_sta, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__Bool));

    publisher_init(&g_crash, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/touch_edge_1", &crash_sta, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__Bool));

    publisher_init(&g_temperature, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/temperature", &temperature, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__Float32));

    publisher_init(&g_runTaskCondition, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, RunTaskCondition), "/run_task_condition_pub",
                   &runTaskCondition, BEST, OVERWRITE, sizeof(chassis_interfaces__msg__RunTaskCondition));

    subscrption_init(&g_temperature_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/temperature/update", &temperature_update,
                     DEFAULT, temperature_update_subscription_callback);

    subscrption_init(&g_emerg_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/emerg/update", &emerg_update, DEFAULT,
                     emerg_update_subscription_callback);

    subscrption_init(&g_crash_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/touch_edge/update", &crash_update, DEFAULT,
                     crash_update_subscription_callback);

    /* 目前只有急停 */
    subscrption_init(&g_runTaskCondition_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/robot/state_publisher/work_status/update",
                     &runTaskCondition_update, DEFAULT, emerg_update_subscription_callback);

    sys_exc.data.data     = sys_exc_buffer;
    sys_exc.data.size     = 0;
    sys_exc.data.capacity = sizeof(sys_exc_buffer);

    runTaskCondition.condition_type.data     = runTaskCondition_type;
    runTaskCondition.condition_type.size     = 0;
    runTaskCondition.condition_type.capacity = sizeof(runTaskCondition_type);

    auto_on_timer = osTimerNew(auto_on_cb, osTimerOnce, NULL, NULL);

    mutex_touch_edge_data_upload = osMutexNew(&mutex_touch_edge_data_upload_attr);

    SFU_BOOT_ManageResetSources();

    if (e_wakeup_source_id != SFU_RESET_HW_RESET) {
        sys_exc_set(EXC52_MCU_REBOOT);
    }

    cjson_hooks.malloc_fn = pvPortMalloc;
    cjson_hooks.free_fn   = vPortFree;

    cJSON_InitHooks(&cjson_hooks);

    osThreadNew(task_fal_misc_public_run, NULL, &task_misc_public_attributes);

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_misc_deInit
 * Description:    释放 fal_misc 资源
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
int fal_misc_deInit(void) {
    return 0;
}

void task_fal_misc_run(void *argument) {
    uint32_t thtb_update_ts          = 0;
    uint32_t touch_edge_update_ts    = 0;
    uint32_t selftest_update_ts      = 0;
    uint32_t crash_detect_disable_ts = 0;
    bool     is_crash_can_set        = true;

    fal_misc_init();

    if (sys_paras.auto_on_time_ms != 0XFFFFFFFF) {
        osTimerStart(auto_on_timer, sys_paras.auto_on_time_ms);
    }

    uint8_t emg_io_cnt = 0, crash_io_cnt = 0, flood_io_cnt = 0, rain_io_cnt = 0;
    // char *cjson_string = NULL;
    // cjson_string = create_monitor();

    for (;;) {
        ///< 急停
        ///< 先检查是否已使能
        if (exc_get_item(sys_paras.sys_exc_mask, EXC65_EMERG)) {
            if (HAL_GPIO_ReadPin(MX_IO_SENSOR_EMG_GPIO_Port, MX_IO_SENSOR_EMG_Pin) == GPIO_PIN_SET) {
                emg_io_cnt++;

                if (emg_io_cnt >= IO_SHAKE_CNT_MAX) {
                    emg_io_cnt = 0;

                    if (sys_exc_get(EXC65_EMERG) != true) {
                        bsp_led_on(LED_EMERG);
                        sys_exc_set(EXC65_EMERG);

                        emerg_pub_flag = 1;

                        log_i("Emergency stop set!");
                    }
                }
            } else {
                emg_io_cnt = 0;

                if (sys_exc_get(EXC65_EMERG) != false) {
                    bsp_led_off(LED_EMERG);
                    sys_exc_clear(EXC65_EMERG);
                    emerg_pub_flag = 1;
                    log_i("Emergency stop clear!");
                }
            }
        }

        ///< 碰撞
        ///< 先检查是否已使能
        if (exc_get_item(sys_paras.sys_exc_mask, EXC67_CRASH) && osKernelGetTickCount() >= crash_detect_disable_ts) {
            if (HAL_GPIO_ReadPin(MX_IO_SENSOR_CRASH_GPIO_Port, MX_IO_SENSOR_CRASH_Pin) == GPIO_PIN_RESET) {
                crash_io_cnt++;

                if (crash_io_cnt >= IO_SHAKE_CNT_MAX) {
                    crash_io_cnt = 0;

                    if (is_crash_can_set && sys_exc_get(EXC67_CRASH) != true) {
                        sys_exc_set(EXC67_CRASH);
                        /* 碰撞状态置位，发布两秒钟 */
                        crash_pub_end_ts = osKernelGetTickCount() + 2000;
                        crash_pub_flag   = 1;

                        /* 禁止碰撞检测两秒，避免弹开后立刻清空标志 */
                        crash_detect_disable_ts = osKernelGetTickCount() + 2000;

                        /* 要检测到松开后才能再次置位碰撞异常 */
                        is_crash_can_set = false;

                        log_i("Crash event set!");
                    } else {
                        ///< 如果打开这个打印，保持碰撞状态，打印会很频繁
                        // log_i("Crash event already set.");
                    }
                }
            } else {
                crash_io_cnt = 0;

                /* 允许置位碰撞异常 */
                is_crash_can_set = true;

                if (sys_exc_get(EXC67_CRASH) != false) {
                    sys_exc_clear(EXC67_CRASH);

                    crash_pub_end_ts = osKernelGetTickCount() + 2000;
                    crash_pub_flag   = 1;
                    log_i("Crash event clear!");
                }
            }
        }

        ///< 水浸
        ///< 先检查是否已使能
        if (exc_get_item(sys_paras.sys_exc_mask, EXC68_FLOOD)) {
            if (HAL_GPIO_ReadPin(MX_IO_SENSOR_FLOOD_GPIO_Port, MX_IO_SENSOR_FLOOD_Pin) == GPIO_PIN_SET) {
                flood_io_cnt++;

                if (flood_io_cnt >= IO_SHAKE_CNT_MAX) {
                    flood_io_cnt = 0;
                    sys_exc_set(EXC68_FLOOD);
                }
            } else {
                flood_io_cnt = 0;
                sys_exc_clear(EXC68_FLOOD);
            }
        }

        ///< 雨水
        ///< 先检查是否已使能
        if (exc_get_item(sys_paras.sys_exc_mask, EXC69_RAIN)) {
            if (HAL_GPIO_ReadPin(MX_IO_SENSOR_RAIN1_GPIO_Port, MX_IO_SENSOR_RAIN1_Pin) == GPIO_PIN_SET &&
                HAL_GPIO_ReadPin(MX_IO_SENSOR_RAIN2_GPIO_Port, MX_IO_SENSOR_RAIN2_Pin) == GPIO_PIN_SET) {
                rain_io_cnt++;

                if (rain_io_cnt >= IO_SHAKE_CNT_MAX) {
                    rain_io_cnt = 0;
                    sys_exc_set(EXC69_RAIN);
                }
            } else {
                rain_io_cnt = 0;
                sys_exc_clear(EXC69_RAIN);
            }
        }

        /*温湿度传感器*/
        if (osKernelGetTickCount() > thtb_update_ts && fal_pmu_get_sta() == PMU_STA_RUNNING) {
            static uint8_t temperature_inner_last = 0;
            bsp_ntc_get_data(&robot_thtb.temperature_inner);
            if (temperature_inner_last != robot_thtb.temperature_inner) {
                temperature_inner_last = robot_thtb.temperature_inner;
                SYS_SET_BIT(temperature_flag, TEMP_PUB_BIT);
            }
            robot_thtb.humidity_inner      = 0.0;
            robot_thtb.temperature_outside = 0.0;
            robot_thtb.humidity_outside    = 0.0;
            /*5min*/
            thtb_update_ts = osKernelGetTickCount() + 1 * 60 * 1000;
        }

        /*触边传感器*/
        if (osKernelGetTickCount() > touch_edge_update_ts && fal_pmu_get_sta() == PMU_STA_RUNNING) {
            osMutexAcquire(mutex_touch_edge_data_upload, osWaitForever);

            bsp_tough_edge_get_data(&robot_tough_edge.front_adc_val, &robot_tough_edge.front_distance_val, &robot_tough_edge.back_adc_val,
                                    &robot_tough_edge.back_distance_val);

            fal_infrared_data_public(robot_tough_edge.front_distance_val, &uros_infrared[0], g_uros_infrared[0]);
            fal_infrared_data_public(robot_tough_edge.back_distance_val, &uros_infrared[1], g_uros_infrared[1]);

            if (bsp_get_tough_edge_error_flag()) {
                sys_exc_set(EXC51_NVG_TOUGH_EDGE);
            } else {
                sys_exc_clear(EXC51_NVG_TOUGH_EDGE);
            }

            osMutexRelease(mutex_touch_edge_data_upload);
            /*50ms  20Hz*/
            touch_edge_update_ts = osKernelGetTickCount() + 50;
        }

        ///< 检查 导航板 SOC 通信状态
        if (osKernelGetTickCount() > nvg_soc_life_ts && fal_pmu_get_sta() == PMU_STA_RUNNING) {
            sys_exc_set(EXC35_NVG_SOC_COM_TO);
            motor_speed_vw_set(0, 0);
        } else {
            sys_exc_clear(EXC35_NVG_SOC_COM_TO);
        }

        ///< 自检状态更新
        if (osKernelGetTickCount() > selftest_update_ts) {
            selftest_sta_update();
            selftest_update_ts = osKernelGetTickCount() + 1000;
        }

        HAL_WWDG_Refresh(&hwwdg);

        osDelay(2);
    }
}

void task_fal_misc_public_run(void *argument) {
    uint32_t sys_exc_pub_ts = 0;

    uint32_t crash_pub_ts = 0;

    for (;;) {
        // log_i("task_fal_misc_public_run...");

        ///< 发布异常检测状态
        if (osKernelGetTickCount() > sys_exc_pub_ts) {
            char *cjson_string = NULL;
            cjson_string       = create_exc_json_string();
            if (cjson_string != NULL) {
                log_i("[%04d] %s", strlen(cjson_string), cjson_string);

                fal_sys_exc_public(cjson_string);

                vPortFree(cjson_string);
            }

            sys_exc_pub_ts = osKernelGetTickCount() + 1000;
        }

        ///< 发布急停状态
        if (emerg_pub_flag) {
            emerg_sta.data = sys_exc_get(EXC65_EMERG);

            message_publish(&g_emerg);

            emerg_pub_flag = 0;

            log_i("Emerg pub finish. Emerg event clear!");

            if (sys_exc_get(EXC65_EMERG)) {
                sprintf(runTaskCondition.condition_type.data, "emerg");
                runTaskCondition.condition_result = false;
            } else {
                sprintf(runTaskCondition.condition_type.data, "emerg");
                runTaskCondition.condition_result = true;
            }

            message_publish(&g_runTaskCondition);
        }

        ///< 发布安全触边状态
        if (crash_pub_flag && osKernelGetTickCount() > crash_pub_ts) {
            crash_sta.data = sys_exc_get(EXC67_CRASH);

            message_publish(&g_crash);

            if (osKernelGetTickCount() >= crash_pub_end_ts) {
                crash_pub_flag = 0;

                if (sys_exc_get(EXC67_CRASH) != false) {
                    /* 碰撞发布结束， 设置当前状态为非碰撞 */
                    sys_exc_clear(EXC67_CRASH);
                    log_i("Crash pub finish. Crash event clear!");

                    /* 非碰撞状态发布两秒 */
                    crash_pub_flag   = 1;
                    crash_pub_end_ts = osKernelGetTickCount() + 2000;
                }

            } else {
                crash_pub_ts = osKernelGetTickCount() + 100;
            }
        }

        if (SYS_GET_BIT(temperature_flag, TEMP_PUB_BIT)) {
            SYS_CLR_BIT(temperature_flag, TEMP_PUB_BIT);
            temperature.data = robot_thtb.temperature_inner;
            message_publish(&g_temperature);
        }

        osDelay(10);
    }
}

static void selftest_sta_update(void) {
    if (sys_exc_get(EXC49_NVG_EEPROM)) {
        selftest_sta.bit0_eeprom = 1;
    } else {
        selftest_sta.bit0_eeprom = 0;
    }

    if (sys_exc_get(EXC50_NVG_IR)) {
        selftest_sta.bit1_ir = 1;
    } else {
        selftest_sta.bit1_ir = 0;
    }

    if (sys_exc_get(EXC39_IMU_DATA)) {
        selftest_sta.bit2_imu = 1;
    } else {
        selftest_sta.bit2_imu = 0;
    }

    if (sys_exc_get(EXC44_GPS_TO)) {
        selftest_sta.bit3_gps = 1;
    } else {
        selftest_sta.bit3_gps = 0;
    }

    if (sys_exc_get(EXC34_BATT_TO)) {
        selftest_sta.bit4_batt = 1;
    } else {
        selftest_sta.bit4_batt = 0;
    }

    if (sys_exc_get(EXC33_MOTOR_COM_TO)) {
        selftest_sta.bit5_motor_drv = 1;
    } else {
        selftest_sta.bit5_motor_drv = 0;
    }

    if (sys_exc_get(EXC36_TASK_SECUR_TO)) {
        selftest_sta.bit6_task_secur = 1;
    } else {
        selftest_sta.bit6_task_secur = 0;
    }
}

void nvg_soc_live_update(void) {
    nvg_soc_life_ts = osKernelGetTickCount() + NVG_SOC_LIFE_EXTEND;
}

static void auto_on_cb(void *argument) {
    soft_button_click();
}

__attribute__((section(".ccmram"))) char cStringBuffer[2048] = {0};

#if 0

unsigned long getRunTimeCounterValue(void) {
    unsigned long ts;
    uint32_t      ts_l;

    HAL_SuspendTick();

    ts = HAL_GetTick();

    ts_l = __HAL_TIM_GET_COUNTER(&htim6);

    ts = ts * 10 + ts_l / 100;

    HAL_ResumeTick();

    return ts;
}

BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *const pcHeader =
        "   State   Prior   Stack   "
        "ID\r\n**********************************************************\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
    "Task" so the column lines up with the centre of the heading. */
    configASSERT(configMAX_TASK_NAME_LEN > 3);

    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskList(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

/*-----------------------------------------------------------*/

#if (configGENERATE_RUN_TIME_STATS == 1)

BaseType_t prvRunTimeStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *const pcHeader =
        "   Time(*100us)    "
        "Time(%)\r\n*******************************************************"
        "\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Pad the string "task" with however many bytes necessary to make it the
    length of a task name.  Minus three for the null terminator and half the
    number of characters in "Task" so the column lines up with the centre of
    the heading. */
    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskGetRunTimeStats(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

#endif /* configGENERATE_RUN_TIME_STATS */
/*-----------------------------------------------------------*/

int show_task_state(void) {
    // uint32_t ts1, ts2;

    // ts1 = HAL_GetTick();
    prvTaskStatsCommand(cStringBuffer, 2048, NULL);
    // ts2 = HAL_GetTick();

    printf("%s\r\n", cStringBuffer);
    // printf("start[%d] end[%d] len[%d]\r\n", ts1, ts2, strlen(cStringBuffer));

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_state, show_task_state, show_task_state);

int show_task_cpu(void) {
    // uint32_t ts1, ts2;

    // ts1 = HAL_GetTick();
    prvRunTimeStatsCommand(cStringBuffer, 2048, NULL);
    // ts2 = HAL_GetTick();
    printf("%s\r\n", cStringBuffer);
    // printf("start[%d] end[%d] len[%d]\r\n", ts1, ts2, strlen(cStringBuffer));
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_cpu, show_task_cpu, show_task_cpu);

int show_heap_state(void) {
    HeapStats_t heapstate;
    vPortGetHeapStats(&heapstate);
    /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    printf("xAvailableHeapSpaceInBytes      : %d\r\n", heapstate.xAvailableHeapSpaceInBytes);
    /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    printf("xSizeOfLargestFreeBlockInBytes  : %d\r\n", heapstate.xSizeOfLargestFreeBlockInBytes);
    /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    printf("xSizeOfSmallestFreeBlockInBytes : %d\r\n", heapstate.xSizeOfSmallestFreeBlockInBytes);
    /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    printf("xNumberOfFreeBlocks             : %d\r\n", heapstate.xNumberOfFreeBlocks);
    /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    printf("xMinimumEverFreeBytesRemaining  : %d\r\n", heapstate.xMinimumEverFreeBytesRemaining);
    /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    printf("xNumberOfSuccessfulAllocations  : %d\r\n", heapstate.xNumberOfSuccessfulAllocations);
    /* The number of calls to vPortFree() that has successfully freed a block of memory. */
    printf("xNumberOfSuccessfulFrees        : %d\r\n", heapstate.xNumberOfSuccessfulFrees);
    printf("\r\n");
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_heap_state, show_heap_state, show_heap_state);

uint8_t read_coredown(void) {
    sys_paras_read_coredown(cStringBuffer);

    printf("\r\ncoredown msg:\r\n");
    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "-"
        "--\r\n");

    printf("%s", cStringBuffer);

    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "-"
        "--\r\n");

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), read_coredown, read_coredown, read_coredown);

extern char *vTaskName(void);

void wwdgtimeout_coredown_save(void) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_WWDG_Refresh(&hwwdg);

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    sprintf(cStringBuffer, "[wwdg timeout at %d-%d-%d %02d:%02d:%02d]\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
            time.Seconds);

    // prvRunTimeStatsCommand(cStringBuffer+strlen(cStringBuffer),
    // 2048-strlen(cStringBuffer), NULL);
    sprintf(cStringBuffer + strlen(cStringBuffer), "Last running task: %s", vTaskName());

    cStringBuffer[strlen(cStringBuffer)] = '\0';

    sys_paras_write_coredown(cStringBuffer, strlen(cStringBuffer) + 1);

    HAL_WWDG_Refresh(&hwwdg);
    sys_paras_wwdg_to_inc();
}

void hardfault_coredown_save(void) {
    HAL_WWDG_Refresh(&hwwdg);

    cStringBuffer[strlen(cStringBuffer)] = '\0';

    sys_paras_write_coredown(cStringBuffer, strlen(cStringBuffer) + 1);

    HAL_WWDG_Refresh(&hwwdg);
    sys_paras_hardfault_inc();
}

void hardfault_coredown_append(const char *format, ...) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    static uint8_t flag = 0;

    if (flag == 0) {
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
        sprintf(cStringBuffer, "[Hardfault at %d-%d-%d %02d:%02d:%02d]\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
                time.Seconds);
        flag = 1;
    }

    HAL_WWDG_Refresh(&hwwdg);

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* 实现数据输出 */
    vsprintf(cStringBuffer + strlen(cStringBuffer), format, args);

    va_end(args);
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg) {
    HAL_WWDG_Refresh(hwwdg);

    wwdgtimeout_coredown_save();

    while (1)
        ;
}
#endif

/**
 * @brief  Return the reset source  detected after a reboot. The related flag is
 * reset at the end of this function.
 * @param  peResetpSourceId: to be filled with the detected source of reset
 * @note   In case of multiple reset sources this function return only one of
 * them. It can be improved returning and managing a combination of them.
 * @retval SFU_SUCCESS if successful, SFU_ERROR otherwise
 */
void SFU_LL_SECU_GetResetSources(SFU_RESET_IdTypeDef *peResetpSourceId) {
    /* Check if the last reset has been generated from a Watchdog exception */
    if ((__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) || (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)) {
        *peResetpSourceId = SFU_RESET_WDG_RESET;

    }

    /* Check if the last reset has been generated from a Low Power reset */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
        *peResetpSourceId = SFU_RESET_LOW_POWER;

    }

    /* Check if the last reset has been generated from a Software reset  */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
        *peResetpSourceId = SFU_RESET_SW_RESET;

    }
    /* Check if the last reset has been generated from a Hw pin reset  */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        *peResetpSourceId = SFU_RESET_HW_RESET;

    }
    /* Unknown */
    else {
        *peResetpSourceId = SFU_RESET_UNKNOWN;
    }
}

/**
 * @brief  Clear the reset sources. This function should be called after the
 * actions on the reset sources has been already taken.
 * @param  none
 * @note   none
 * @retval none
 */
void SFU_LL_SECU_ClearResetSources() {
    /* Clear reset flags  */
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

#define TRACE log_i

/**
 * @brief  Manage the  the Reset sources, and if the case store the error for
 * the next steps
 * @param  None
 * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
 */
static void SFU_BOOT_ManageResetSources(void) {
    // SFU_ErrorStatus     e_ret_status = SFU_SUCCESS;
    // SFU_RESET_IdTypeDef e_wakeup_source_id = SFU_RESET_UNKNOWN;
    // SFU_EXCPT_IdTypeDef e_exception;

    /* Check the wakeup sources */

#ifdef IAP
    struct fdb_blob blob;
    fdb_kv_get_blob(&kvdb, "wakeup_src", fdb_blob_make(&blob, (uint8_t *) &e_wakeup_source_id, sizeof(e_wakeup_source_id)));
#else
    SFU_LL_SECU_GetResetSources(&e_wakeup_source_id);
#endif

    switch (e_wakeup_source_id) {
            /*
             * Please note that the example of reset causes handling below is
             * only a basic example to illustrate the way the RCC_CSR flags can
             * be used to do so. It is based on the behaviors we consider as
             * normal and abnormal for the SB_SFU and UserApp example projects
             * running on a Nucleo HW board. Hence this piece of code must
             * systematically be revisited and tuned for the targeted system
             * (software and hardware expected behaviors must be assessed to
             * tune this code).
             *
             * One may use the "uExecID" parameter to determine if the last exec
             * status was in the SB_SFU context or UserApp context to implement
             * more clever checks in the reset cause handling below.
             */

        case SFU_RESET_WDG_RESET:
            TRACE("WARNING: A Reboot has been triggered by a Watchdog reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* This event has to be considered as an error to manage */
            // e_exception = SFU_EXCPT_WATCHDOG_RESET;
            break;

        case SFU_RESET_LOW_POWER:
            TRACE("INFO: A Reboot has been triggered by a LowPower reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_HW_RESET:
            TRACE("INFO: A Reboot has been triggered by a Hardware reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. This is because a Nucleo board offers a RESET
               button triggering the HW reset. But this is strictly related to
               the final system. If needed to be managed as an error please add
               the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_BOR_RESET:
            TRACE("INFO: A Reboot has been triggered by a BOR reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_SW_RESET:
            TRACE("INFO: A Reboot has been triggered by a Software reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage, also because a sw reset is generated when the
               State Machine forces a Reboot. But this is strictly related to
               the final system. If needed to be managed as an error please add
               the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_OB_LOADER:
            TRACE(
                "WARNING: A Reboot has been triggered by an Option Bytes "
                "reload!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage, also because an OptionByte loader is called
               after applying some of the security protections (see
               SFU_CheckApplyStaticProtections). But this is strictly related to
               the final system. If needed to be managed as an error please add
               the right error code in the following src code line. Typically we
               may implement a more clever check where we determine if this OB
               reset occurred only once because SB_SFU had to tune the OB
               initially, or if it occurred again after these initial settings.
               If so, we could consider it as an attack. */
            // e_exception = SFU_EXCPT_NONE;
            break;

        default:
            TRACE(
                "WARNING: A Reboot has been triggered by an Unknown reset "
                "source!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;
    }

    /* Once the reset sources has been managed and a possible error has been
     * set, clear the reset sources */
    SFU_LL_SECU_ClearResetSources();
}

static uint8_t sys_reboot_source(void) {
    printf("\r\n");

    switch (e_wakeup_source_id) {
        case SFU_RESET_WDG_RESET:
            printf("WARNING: A Reboot has been triggered by a Watchdog reset!");

            break;

        case SFU_RESET_LOW_POWER:
            printf("INFO: A Reboot has been triggered by a LowPower reset!");

            break;

        case SFU_RESET_HW_RESET:
            printf("INFO: A Reboot has been triggered by a Hardware reset!");

            break;

        case SFU_RESET_BOR_RESET:
            printf("INFO: A Reboot has been triggered by a BOR reset!");

            break;

        case SFU_RESET_SW_RESET:
            printf("INFO: A Reboot has been triggered by a Software reset!");

            break;

        case SFU_RESET_OB_LOADER:
            printf(
                "WARNING: A Reboot has been triggered by an Option Bytes "
                "reload!");

            break;

        default:
            printf(
                "WARNING: A Reboot has been triggered by an Unknown reset "
                "source!");

            break;
    }

    printf("\r\n");

    return e_wakeup_source_id;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys_reboot_source, sys_reboot_source, sys_reboot_source);

#include "system_status.h"

extern sys_rcc_flag_info_stu_t sys_rcc_flag_info;
void                           check_rcc_flag(void) {
    switch (e_wakeup_source_id) {
        case SFU_RESET_WDG_RESET:
            sys_rcc_flag_info.flag |= (1 << 3);
            break;

        case SFU_RESET_LOW_POWER:
            sys_rcc_flag_info.flag |= (1 << 6);
            break;

        case SFU_RESET_HW_RESET:
            sys_rcc_flag_info.flag |= (1 << 1);
            break;

        case SFU_RESET_BOR_RESET:

            break;

        case SFU_RESET_SW_RESET:
            sys_rcc_flag_info.flag |= (1 << 2);
            break;

        case SFU_RESET_OB_LOADER:

            break;

        default:

            break;
    }

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
#ifdef RCC_FLAG_BORRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) {
        //这是上电复位或欠压复位
        sys_rcc_flag_info.flag |= (1 << 6);
    }
#endif
}

static void fal_infrared_data_public(float data, sensor_msgs__msg__Range *msg_uros_infrared, publisher pub_uros_infrared) {
    msg_uros_infrared->radiation_type = 2;
    msg_uros_infrared->min_range      = 0.01;
    msg_uros_infrared->max_range      = 0.60;
    msg_uros_infrared->range          = data / 100.0;
    pal_uros_msg_set_timestamp(&msg_uros_infrared->header.stamp);
    message_publish(&pub_uros_infrared);
    return;
}

static void fal_sys_exc_public(char *cjson_string) {
    if (strlen(cjson_string) <= sizeof(sys_exc_buffer)) {
        strcpy(sys_exc.data.data, cjson_string);
        sys_exc.data.size = strlen(sys_exc.data.data);
        message_publish(&g_sys_exc);
    }

    return;
}

static char *create_exc_json_string(void) {
    cJSON * pJsonRoot  = NULL;
    cJSON * pJsonChild = NULL;
    char *  lpJsonStr  = NULL;
    uint8_t error_num  = 0;

    pJsonRoot  = cJSON_CreateObject();
    pJsonChild = cJSON_CreateObject();

    if (pJsonRoot != NULL && pJsonChild != NULL) {
        if (sys_exc_get(EXC33_MOTOR_COM_TO)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "motor_com_to", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "motor_com_to", 0);
        }

        if (sys_exc_get(EXC34_BATT_TO)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "bms_com_to", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "bms_com_to", 0);
        }

        if (sys_exc_get(EXC35_NVG_SOC_COM_TO)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "speed_cmd_to", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "bms_com_to", 0);
        }

        if (sys_exc_get(EXC39_IMU_DATA)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "imu_com_to", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "bms_com_to", 0);
        }

        if (sys_exc_get(EXC49_NVG_EEPROM)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "eeprom_rw", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "eeprom_rw", 0);
        }

        if (sys_exc_get(EXC50_NVG_IR)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "charge_ir", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "charge_ir", 0);
        }

        if (sys_exc_get(EXC51_NVG_TOUGH_EDGE)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "touch_ir", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "touch_ir", 0);
        }

        if (sys_exc_get(EXC52_MCU_REBOOT)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "mcu_reboot", e_wakeup_source_id);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "mcu_reboot", 0);
        }

        if (sys_exc_get(EXC65_EMERG)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "emergency", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "emergency", 0);
        }

        if (sys_exc_get(EXC67_CRASH)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "crash", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "crash", 0);
        }

        if (sys_exc_get(EXC68_FLOOD)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "flood", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "flood", 0);
        }

        if (sys_exc_get(EXC70_LOW_POWER)) {
            error_num++;
            cJSON_AddNumberToObject(pJsonChild, "power_low", 1);
        } else {
            // cJSON_AddNumberToObject(pJsonChild, "power_low", 0);
        }

        cJSON_AddNumberToObject(pJsonRoot, "error_num", error_num);
        cJSON_AddItemToObject(pJsonRoot, "details", pJsonChild);

        lpJsonStr = cJSON_PrintUnformatted(pJsonRoot);

        cJSON_Delete(pJsonRoot);
    }

    return lpJsonStr;
}

static void temperature_update_subscription_callback(const void *msgin) {
    log_i("/temperature/update");
    SYS_SET_BIT(temperature_flag, TEMP_PUB_BIT);
}

static void emerg_update_subscription_callback(const void *msgin) {
    log_i("/emerg/update");
    emerg_pub_flag = 1;
}

static void crash_update_subscription_callback(const void *msgin) {
    log_i("/crash/update");
    crash_pub_flag = 1;
}

#ifdef __cplusplus
}
#endif

/* @} FAL_MISC */
/* @} Robot_FAL */
