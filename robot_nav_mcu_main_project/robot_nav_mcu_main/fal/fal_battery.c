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
#include "fal_battery.h"
#include "cmsis_os.h"
#include "bsp_battery.h"
#include "sys_pubsub.h"
#include "sys_paras.h"
#include "sys_exc.h"
#include "pal_uros.h"
#include "sensor_msgs/msg/battery_state.h"
#include "std_msgs/msg/empty.h"
#include "std_msgs/msg/bool.h"
#define LOG_TAG "fal_battery"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_BATTERY 电池管理功能 - FAL_BATTERY
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define BATT_LIVE_TIME_MS 5000
#define BATT_PUB_BIT      1

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern BATTERY_BMS_DATA_T battery_bms;

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
static uint32_t batt_live_ts = 0;
static uint8_t  battery_flag = 0;

///< battery
static publisher                      g_battery;
static sensor_msgs__msg__BatteryState battery;
char                                  battery_buffer[10] = "battery";

///< sub battery_update
static subscrption         g_battery_update;
static std_msgs__msg__Bool battery_update;

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
static void fal_battery_status_public(void);
static void battery_update_subscription_callback(const void *msgin);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_battery_init
 * Description:    初始化fal_battery_init层
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
int fal_battery_init(void) {
    subscrption_init(&g_battery_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/battery/update", &battery_update, DEFAULT,
                     battery_update_subscription_callback);

    publisher_init(&g_battery, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "/battery", &battery, DEFAULT, SEND,
                   sizeof(sensor_msgs__msg__BatteryState));

    battery.header.frame_id.data     = battery_buffer;
    battery.header.frame_id.capacity = sizeof(battery_buffer);
    battery.header.frame_id.size     = strlen(battery_buffer);

    ///< 电池接口初始化
    bsp_battery_init();

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
int fal_battery_deInit(void) {
    return 0;
}

uint8_t get_current_percentage(void) {
    return 1.0f * battery_bms.cap_remain / battery_bms.cap_full_in;
}

void task_fal_battery_run(void *argument) {
    static uint8_t  percentage_last          = 0;
    static uint16_t power_supply_status_last = 0;
    BATT_STA_T      batt_sta;

    fal_battery_init();

    uint8_t batt_percent;

    bool is_batt_low_detected = false;

    osDelay(1000);

    batt_live_ts = osKernelGetTickCount() + BATT_LIVE_TIME_MS;

    uint8_t bms_retry        = 0;
    uint8_t is_bms_update_ok = 0;

    uint32_t bms_update_err_cnt     = 0;
    uint32_t bms_update_err_exc_cnt = 0;

    for (;;) {
        // if(fal_pmu_get_sta() != PMU_STA_STANDBY)
        bms_retry        = 0;
        is_bms_update_ok = 0;

        while (bms_retry < 5) {
            if (bsp_battery_bms_update_it(100) == 0) {
                is_bms_update_ok = 1;
                break;
            }

            bms_retry++;
        }

        if (is_bms_update_ok) {
            batt_live_ts = osKernelGetTickCount() + BATT_LIVE_TIME_MS;

            batt_percent = bsp_battery_volatage_percent_get();

            log_i(
                "Battery info update ok, percentage[%d], charge[%d] err_cnt[%d], "
                "exc_cnt[%d]",
                batt_percent, bsp_battery_charge_sta_get(), bms_update_err_cnt, bms_update_err_exc_cnt);

            if (sys_paras.battery_low_protect != 0 && (batt_percent <= sys_paras.battery_low_protect) && bsp_battery_charge_sta_get() == 0) {
                if (is_batt_low_detected == false) {
                    log_w("Battery percentage low detect. Percent[%d].", batt_percent);

                    sys_exc_set(EXC70_LOW_POWER);

                    batt_sta.sta = BATT_LOW_DETECT;
                    pub_topic(SYS_EVT_BATT_STA, &batt_sta);

                    is_batt_low_detected = true;
                }

            } else {
                sys_exc_clear(EXC70_LOW_POWER);

                /*
                if(is_batt_low_detected == true)
                {
                    log_i("Battery percentage low cancel.");

                    sys_exc_clear(EXC70_LOW_POWER);

                    batt_sta.sta = BATT_LOW_CANCEL;
                    pub_topic(SYS_EVT_BATT_STA, &batt_sta);

                    is_batt_low_detected = false;
                }
                */
            }
        } else {
            bms_update_err_cnt++;

            log_e("Battery info update error.");
        }

        if (fal_pmu_get_sta() == PMU_STA_RUNNING) {
            is_batt_low_detected = false;
        }

        if (osKernelGetTickCount() >= batt_live_ts) {
            bms_update_err_exc_cnt++;
            sys_exc_set(EXC34_BATT_TO);
        } else {
            sys_exc_clear(EXC34_BATT_TO);
        }

        // <<< 比较电量信息，变动触发上报
        uint8_t percentage = get_current_percentage();

        if (percentage_last != percentage || power_supply_status_last != battery_bms.sta_bms.charge_sta) {
            percentage_last          = percentage;
            power_supply_status_last = battery_bms.sta_bms.charge_sta;
            SYS_SET_BIT(battery_flag, BATT_PUB_BIT);
        }

        if (SYS_GET_BIT(battery_flag, BATT_PUB_BIT)) {
            SYS_CLR_BIT(battery_flag, BATT_PUB_BIT);
            fal_battery_status_public();
        }

        osDelay(1000);
    }
}

static void fal_battery_status_public(void) {
    if (sys_exc_get(EXC34_BATT_TO)) {
        battery.present = false;
    } else {
        battery.present = true;
    }

    battery.voltage         = 1.0 * battery_bms.voltage / 1000;
    battery.temperature     = 1.0 * battery_bms.temp_cell1;
    battery.current         = 1.0 * battery_bms.current / 1000;
    battery.charge          = 1.0 * battery_bms.current / 1000;
    battery.capacity        = 1.0 * battery_bms.cap_full_in / 1000;
    battery.design_capacity = 1.0 * battery_bms.cap_full_in / 1000;

    if (battery_bms.sta_bms.charge_sta) {
        battery.power_supply_status = sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_CHARGING;
    } else {
        battery.power_supply_status = sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_NOT_CHARGING;
    }

    battery.power_supply_health     = sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_UNKNOWN;
    battery.power_supply_technology = sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_UNKNOWN;
    battery.percentage              = 1.0 * battery_bms.cap_remain / battery_bms.cap_full_in;

    pal_uros_msg_set_timestamp(&battery.header.stamp);
    message_publish(&g_battery);
}

void battery_update_subscription_callback(const void *msgin) {
    log_i("/battery/update");
    SYS_SET_BIT(battery_flag, BATT_PUB_BIT);
}

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_407 */
