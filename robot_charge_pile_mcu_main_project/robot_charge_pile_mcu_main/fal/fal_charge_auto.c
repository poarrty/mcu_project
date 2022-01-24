/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_charge_auto.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-13 20:35:09
 * @Description: 自动充电相关接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_charge_auto.h"
#include "task_charge.h"
#include "task_digital_tube.h"
#include "task_led.h"
#include "common_def.h"
#include <string.h>
#include <rcl/rcl.h>
#include "pal_uros.h"
#include "std_msgs/msg/bool.h"
#include "std_srvs/srv/set_bool.h"
#include "std_srvs/srv/trigger.h"
#include "sensor_msgs/msg/battery_state.h"
#include "drv_gpio.h"
#include "log.h"
#include "cmsis_os.h"
#include "queue.h"
#include "bsp_clean_fun.h"

charge_auto_charge_var_stu_t auto_charge_var;

// static publisher g_electrode;
// std_msgs__msg__Bool electrode;

static publisher g_battery_update;
std_msgs__msg__Bool battery_update;

static service g_pile_set_charge;
std_srvs__srv__SetBool_Request pile_set_charge_req;
std_srvs__srv__SetBool_Response pile_set_charge_res;

static subscrption g_battery;
sensor_msgs__msg__BatteryState battery;
static char battery_buffer[10];

extern osEventFlagsId_t sys_event_uros_timeout;
void pile_set_charge(const void *req, void *res);
void get_battery_info(const void *msgin);
/******************************************************************************
 * @Function: fal_charge_auto_charge_init
 * @Description: 自动充电初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_charge_auto_charge_init(void) {
    memset(&auto_charge_var, 0, sizeof(charge_auto_charge_var_stu_t));

    // publisher_init(&g_electrode,
    // 			   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    // 			   "/auto_charge/pile_electrode_station",
    // 			   &electrode,
    // 			   DEFAULT,
    // 			   OVERWRITE,
    // 			   sizeof(std_msgs__msg__Bool)
    // 			  );

    publisher_init(&g_battery_update,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty),
                   "/battery/update", &battery_update, DEFAULT, OVERWRITE,
                   sizeof(std_msgs__msg__Bool));

    subscrption_init(
        &g_battery, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        "/battery", &battery, DEFAULT, get_battery_info);

    service_init(&g_pile_set_charge,
                 ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
                 "/auto_charge/pile_set_charge", &pile_set_charge_req,
                 &pile_set_charge_res, BEST, pile_set_charge);

    battery.header.frame_id.data = battery_buffer;
    battery.header.frame_id.capacity = sizeof(battery_buffer);
    battery.header.frame_id.size = 0;
}

void pile_set_charge(const void *req, void *res) {
    std_srvs__srv__SetBool_Request *req_in =
        (std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response *res_in =
        (std_srvs__srv__SetBool_Response *) res;

    LOG_DEBUG("Service pile_set_charge request value: %d.\n", req_in->data);

    if (!SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT)) {
        drv_gpio_auto_charge_set(req_in->data);

        if (req_in->data == 1) {
            SYS_SET_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);
        } else {
            SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);
        }
    }
    res_in->success = 1;
}

void get_battery_info(const void *msgin) {
    const sensor_msgs__msg__BatteryState *msg =
        (const sensor_msgs__msg__BatteryState *) msgin;

    LOG_DEBUG("battert current : %1f ,percentage : %3f, status: %d",
              msg->current, msg->percentage, msg->power_supply_status);
    uint8_t battery_percentage = (uint8_t)(msg->percentage * 100.0 + 0.5);

    if ((SYS_GET_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT)) &&
        (!SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT))) {
        if (msg->power_supply_status == POWER_SUPPLY_STATUS_CHARGING) {
            digital_tube_display_on(battery_percentage, DYNAMIC_DISPLAY,
                                    BATTERY_DISPLAY);
        } else {
            digital_tube_display_on(battery_percentage, STATIC_DISPLAY,
                                    BATTERY_DISPLAY);
        }
    }
}

/******************************************************************************
 * @Function: charge_auto_charge_server
 * @Description: 自动充电服务
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_charge_auto_charge_server(void) {
    static uint32_t battery_update_ts;
    // static uint32_t publisher_electrode_down_ts;

    // if (SYS_GET_BIT(auto_charge_var.flag, PUBLISH_ELECTRODE_UP_BIT))
    // {
    // 	SYS_CLR_BIT(auto_charge_var.flag, PUBLISH_ELECTRODE_UP_BIT);
    // 	auto_charge_var.electrode_down_send_times = 0;
    // 	// SYS_CLR_BIT(auto_charge_var.flag, PUBLISH_ELECTRODE_DOWN_BIT);
    // }

    // if(auto_charge_var.electrode_down_send_times  > 0)
    // {
    // 	if(osKernelGetTickCount() > publisher_electrode_down_ts + 500)
    // 	{
    // 		publisher_electrode_down_ts = osKernelGetTickCount()+500;
    // 		electrode.data = 1;
    // 		if (message_publish(&g_electrode) == 0)
    // 		{
    // 			auto_charge_var.electrode_down_send_times--;
    // 		}
    // 	}
    // }

    if (SYS_GET_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT)) {
        if (osKernelGetTickCount() > battery_update_ts + 1000) {
            battery_update_ts = osKernelGetTickCount() + 1000;
            battery_update.data = 1;
            message_publish(&g_battery_update);
        }
    }
}

void uros_timeout_server(void) {
    uint32_t sub_evt = osEventFlagsWait(
        sys_event_uros_timeout, 1 << SYNC_STATUS_RECONNECT, osFlagsWaitAny, 1);

    if ((sub_evt & osFlagsError) == 0) {
        drv_gpio_charge_off();
        all_clean_fun_stop();
        if (SYS_GET_BIT(auto_charge_var.flag, CHARGE_AUTO_ELECTRODE_DOWN_BIT)) {
            digital_tube_display_on(DIGITAL_TUBE_UROS_TIMEOUT, STATIC_DISPLAY,
                                    FUNCTION_CODE_DISPLAY);
        }
    }
}

// void electrode_up_publish(void)
// {
// 	electrode.data = 0;
// 	message_publish(&g_electrode);
// }
