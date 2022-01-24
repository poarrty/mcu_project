#include "task_workstation_clean.h"
#include "common_def.h"
#include <string.h>
#include <rcl/rcl.h>
#include "pal_uros.h"
#include "std_msgs/msg/bool.h"
#include "std_srvs/srv/set_bool.h"
#include "std_srvs/srv/trigger.h"
#include "sensor_msgs/msg/battery_state.h"
#include "std_msgs/msg/string.h"
#include "std_msgs/msg/bool.h"
#include "drv_gpio.h"
#include "log.h"
#include "cmsis_os.h"
#include "queue.h"
#include "bsp_clean_fun.h"
#include "fal_charge_auto.h"
#include "task_digital_tube.h"
#include "cJSON.h"

#include "shell.h"

static workstation_state_stu_t workstation_state_var;

#if 1
static service service_clean_water;
static std_srvs__srv__SetBool_Request workstation_add_clean_water_req;
static std_srvs__srv__SetBool_Response workstation_add_clean_water_res;

static service service_sewage_water;
static std_srvs__srv__SetBool_Request workstation_sewage_water_req;
static std_srvs__srv__SetBool_Response workstation_sewage_water_res;

static service service_self_clean;
static std_srvs__srv__SetBool_Request workstation_self_clean_req;
static std_srvs__srv__SetBool_Response workstation_self_clean_res;

static publisher pub_transition_tank_status;
static std_msgs__msg__UInt8 transition_tank_status;

static publisher g_workstation_state;
static std_msgs__msg__String workstation_state_info;

static subscrption g_workstation_state_update;
static std_msgs__msg__Bool workstation_state_update;

static void workstation_add_clean_water_handle(const void *req, void *res);
static void workstation_sewage_water_handle(const void *req, void *res);
static void workstation_self_clean_handle(const void *req, void *res);

static subscrption g_water_position;
std_msgs__msg__String water_position_info;
void get_water_position_info(const void *msgin);
void workstation_update_req(const void *msgin);
#endif

uint8_t water_level_full_timeout;  //满水超时

void workstation_clean_fun_init(void) {
#if 1
    service_init(&service_clean_water,
                 ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
                 "/workstation/add_clean_water",
                 &workstation_add_clean_water_req,
                 &workstation_add_clean_water_res, BEST,
                 workstation_add_clean_water_handle);

    service_init(&service_sewage_water,
                 ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
                 "/workstation/sewage_water", &workstation_sewage_water_req,
                 &workstation_sewage_water_res, BEST,
                 workstation_sewage_water_handle);

    service_init(&service_self_clean,
                 ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
                 "/workstation/self_cleaning", &workstation_self_clean_req,
                 &workstation_self_clean_res, BEST,
                 workstation_self_clean_handle);

    publisher_init(&pub_transition_tank_status,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
                   "/workstation/transition_tank_status",
                   &transition_tank_status, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__UInt8));

    publisher_init(&g_workstation_state,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
                   "/workstation/state", &workstation_state_info, DEFAULT,
                   OVERWRITE, sizeof(std_msgs__msg__String));

    subscrption_init(
        &g_water_position, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "/water_position", &water_position_info, BEST, get_water_position_info);

    subscrption_init(&g_workstation_state_update,
                     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty),
                     "/workstation/state/update", &workstation_state_update,
                     BEST, workstation_update_req);

    MallocString(&workstation_state_info.data, REQ, 128);
    MallocString(&water_position_info.data, REQ, 256);

#endif
}

void workstation_state_update_handle(char *msg) {
    // 	加水-watering 排水-draining
    // 自清洁-cleaning  失败-failed 停止-stoping
    if (msg != NULL) {
        workstation_state_info.data.data = msg;
        workstation_state_info.data.size = strlen(msg) + 1;
        workstation_state_info.data.capacity = workstation_state_info.data.size;
        message_publish(&g_workstation_state);
    }
}

/******************************************************************************
 * @Function: add_water_init
 * @Description: 加水初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void add_water_init(void) {
    memset(&add_water_var, 0, sizeof(add_water_var_stu_t));
    memset(&pump_pwm, 0, sizeof(pump_pwm_stu_t));
}

/******************************************************************************
 * @Function: task_add_water_init
 * @Description: 加水任务初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int task_add_water_init(void) {
    add_water_init();

    return 0;
}

/******************************************************************************
 * @Function: set_cleaner_diaphragm_pump_timeout
 * @Description: 清洁剂隔膜泵超时时间设置函数入口
 * @Input: value:超时时间,单位秒
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void set_cleaner_diaphragm_pump_timeout(uint16_t value) {
    add_water_var.cleaner_diaphragm_pump_timeout = value;
}

/******************************************************************************
 * @Function: set_transition_box_clean_timeout
 * @Description: 过渡箱清洗超时时间设置函数入口
 * @Input: value:超时时间,单位秒
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void set_transition_box_clean_timeout(uint16_t value) {
    add_water_var.transition_box_clean_timeout = value;
}

void add_water_handle(void) {
    static uint8_t sewage_pump_cnt;
    static uint8_t sewage_pump_start_delay;
    static uint8_t water_level_check_delay;

    sewage_pump_cnt++;

    if (sewage_pump_start_delay > 0) {
        sewage_pump_start_delay--;
    }

    if (water_level_check_delay > 0) {
        water_level_check_delay--;
    }

    if ((SYS_GET_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG)) &&
        (sewage_pump_start_delay == 0)) {
        SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
        sewage_pump_start();
        osDelay(2);
    }

    else if (SYS_GET_BIT(add_water_var.add_water_status,
                         ADD_WATER_PUMP_START_FLAG)) {
        SYS_CLR_BIT(add_water_var.add_water_status, ADD_WATER_PUMP_START_FLAG);
        add_water_pump_start();
        osDelay(2);
    }

    if ((SYS_GET_BIT(auto_charge_var.flag, CL_WATER_LEVEL_FULL_BIT)) &&
        (water_level_check_delay == 0) &&
        (SYS_GET_BIT(add_water_var.add_water_status, SEWAGE_PUMP))) {
        water_level_full_timeout++;

        if (water_level_full_timeout == 3) {
            LOG_DEBUG("%s:[water_level_full_timeout]", __FUNCTION__);
            water_level_full_timeout = 0;
            water_level_check_delay = 15;
            sewage_pump_start_delay = 2;
            sewage_pump_puase();
            SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
        }
    } else {
        water_level_full_timeout = 0;
    }

    if (add_water_var.cleaner_diaphragm_pump_timeout > 0) {
        add_water_var.cleaner_diaphragm_pump_timeout--;

        if (add_water_var.cleaner_diaphragm_pump_timeout ==
            0)  //关闭清洁剂隔膜泵
        {
            CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_OFF();
            CL_TRANSITION_BOX_VALVE_CTRL_TURN_OFF();
        }
    }

    if (add_water_var.sewage_timeout > 0) {
        add_water_var.sewage_timeout--;

        if (add_water_var.sewage_timeout == 0)  //关闭排污水
        {
            sewage_pump_stop();
        }
    }

    if (add_water_var.transition_box_clean_timeout > 0) {
        add_water_var.transition_box_clean_timeout--;

        if (add_water_var.transition_box_clean_timeout == 0)  //过渡箱清洗完成
        {
            add_water_pump_stop();
            ADD_WATER_VALVE_CTRL_TURN_OFF();
            SYS_CLR_BIT(add_water_var.add_water_status, TRANSITION_BOX_CLEAN);
            add_water_var.sewage_timeout = 5;
        }
    }
}

/******************************************************************************
 * @Function: task_workstation_clean_run
 * @Description: 电池任务函数入口
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {void} *pvParameters
 *******************************************************************************/
void task_workstation_clean_run(void *pvParameters) {
    workstation_clean_fun_init();
    task_add_water_init();
    static char *workstation_state[] = {"watering", "draining", "cleaning",
                                        "stoping"};

    while (1) {
        osDelay(1000);
        add_water_handle();

        if (SYS_GET_BIT(add_water_var.add_water_status, SEWAGE_PUMP)) {
            LOG_DEBUG("SEWAGE_PUMP....");
            transition_tank_status.data = 0;
            if (SYS_GET_BIT(auto_charge_var.flag, CL_WATER_LEVEL_FULL_BIT)) {
                transition_tank_status.data = 1;
            }

            message_publish(&pub_transition_tank_status);
        }

        if (workstation_state_var.flag == 1) {
            workstation_state_update_handle(
                workstation_state[workstation_state_var.workstation_state]);
            workstation_state_var.flag = 0;
        }
    }
}

void workstation_update_req(const void *msgin) {
    const bool *msg = (const bool *) msgin;

    // LOG_DEBUG("%d", *msg);
    workstation_state_var.flag = 1;
}

void get_water_position_info(const void *msgin) {
    cJSON *json_msg = NULL;
    cJSON *json_temp = NULL;
    char *string_temp = NULL;

    LOG_DEBUG("%s", __FUNCTION__);

    if (msgin == NULL)
        return;

    const std_msgs__msg__String *string_msg =
        (const std_msgs__msg__String *) msgin;
    if (string_msg->data.data != NULL) {
        json_msg = cJSON_Parse(string_msg->data.data);
        if (json_msg == NULL)
            return;

        json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, "clean_status");
        if (json_temp != NULL) {
            float clean_status = json_temp->valuedouble;
            LOG_DEBUG("clean_status:%f", clean_status);
        }

        json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, "waste_status");
        if (json_temp != NULL) {
            float waste_status = json_temp->valuedouble;
            LOG_DEBUG("waste_status:%f", waste_status);
        }

#if 0
		string_temp = cJSON_Print(json_msg);
		LOG_DEBUG("%s",string_temp);
		if(string_msg != NULL)
		{
			cJSON_free(string_temp);
		}
#endif
        cJSON_Delete(json_msg);
    }
}

static void clean_fun_ctrl(clean_cmd_enum_t cmd) {
    if (SYS_GET_BIT(charge_var, CHARGE_STOP_CHARGE_EN_BIT)) {
        if (cmd & 0x01) {
            LOG_DEBUG("stop key down...");
            return;
        }
    }
    switch (cmd) {
        case IR_RECV_ADD_WATER_START_CMD:
            SYS_SET_BIT(add_water_var.add_water_status,
                        ADD_WATER_PUMP_START_FLAG);

            ADD_WATER_VALVE_CTRL_TURN_ON();
            CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_ON();
            SYS_SET_BIT(add_water_var.add_water_status, ADD_WATER);
            set_cleaner_diaphragm_pump_timeout(
                MAX_CLEANER_DIAPHRAGM_PUMP_TIMEOUT);

            digital_tube_display_on(DIGITAL_TUBE_ADD_WATER_START,
                                    STATIC_DISPLAY, FUNCTION_CODE_DISPLAY);
            break;

        case IR_RECV_ADD_WATER_STOP_CMD:
            add_water_pump_stop();
            ADD_WATER_VALVE_CTRL_TURN_OFF();
            CL_CLEANER_DIAPHRAGM_PUMP_CTRL_TURN_OFF();

            SYS_CLR_BIT(add_water_var.add_water_status, ADD_WATER);
            SYS_CLR_BIT(add_water_var.add_water_status,
                        ADD_WATER_PUMP_START_FLAG);

            set_cleaner_diaphragm_pump_timeout(0);
            digital_tube_display_on(DIGITAL_TUBE_ADD_WATER_STOP, STATIC_DISPLAY,
                                    FUNCTION_CODE_DISPLAY);
            break;

        case IR_RECV_SEWAGE_START_CMD:
            SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
            SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP);

            digital_tube_display_on(DIGITAL_TUBE_SEWAGE_START, STATIC_DISPLAY,
                                    FUNCTION_CODE_DISPLAY);
            break;

        case IR_RECV_SEWAGE_STOP_CMD:
            sewage_pump_stop();
            SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP);
            SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);

            digital_tube_display_on(DIGITAL_TUBE_SEWAGE_STOP, STATIC_DISPLAY,
                                    FUNCTION_CODE_DISPLAY);
            break;

        case IR_RECV_TRANSITION_BOX_CLEAN_START_CMD:
            ADD_WATER_VALVE_CTRL_TURN_OFF();
            SYS_SET_BIT(add_water_var.add_water_status,
                        ADD_WATER_PUMP_START_FLAG);
            SYS_SET_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
            SYS_SET_BIT(add_water_var.add_water_status, TRANSITION_BOX_CLEAN);

            set_transition_box_clean_timeout(MAX_TRANSITION_BOX_CLEAN_TIMEOUT);
            CL_TRANSITION_BOX_VALVE_CTRL_TURN_ON();

            digital_tube_display_on(DIGITAL_TUBE_TRANSITION_BOX_CLEAN_START,
                                    STATIC_DISPLAY, FUNCTION_CODE_DISPLAY);
            break;

        case IR_RECV_TRANSITION_BOX_CLEAN_STOP_CMD:
            ADD_WATER_VALVE_CTRL_TURN_OFF();
            CL_TRANSITION_BOX_VALVE_CTRL_TURN_OFF();
            add_water_pump_stop();
            sewage_pump_stop();

            SYS_CLR_BIT(add_water_var.add_water_status,
                        ADD_WATER_PUMP_START_FLAG);
            SYS_CLR_BIT(add_water_var.add_water_status, SEWAGE_PUMP_START_FLAG);
            SYS_CLR_BIT(add_water_var.add_water_status, TRANSITION_BOX_CLEAN);

            set_transition_box_clean_timeout(0);
            digital_tube_display_on(DIGITAL_TUBE_TRANSITION_BOX_CLEAN_STOP,
                                    STATIC_DISPLAY, FUNCTION_CODE_DISPLAY);
            break;

        default:
            LOG_WARNING("Unsupport cmd!");
            return;
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 clean_fun_ctrl, clean_fun_ctrl, clean_fun_ctrl);

static void workstation_add_clean_water_handle(const void *req, void *res) {
    std_srvs__srv__SetBool_Request *req_in =
        (std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response *res_in =
        (std_srvs__srv__SetBool_Response *) res;

    LOG_DEBUG("%s: [%d].\n", __FUNCTION__, req_in->data);

    if (req_in->data == 0x01) {
        drv_gpio_auto_charge_set(DISABLE);
        SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);

        clean_fun_ctrl(IR_RECV_ADD_WATER_START_CMD);
        workstation_state_var.workstation_state = WATERING;
        workstation_state_var.flag = 1;
    } else {
        clean_fun_ctrl(IR_RECV_ADD_WATER_STOP_CMD);
        workstation_state_var.workstation_state = STOPING;
    }

    res_in->success = 1;
}

static void workstation_sewage_water_handle(const void *req, void *res) {
    std_srvs__srv__SetBool_Request *req_in =
        (std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response *res_in =
        (std_srvs__srv__SetBool_Response *) res;

    LOG_DEBUG("%s: [%d].\n", __FUNCTION__, req_in->data);

    if (req_in->data == 0x01) {
        drv_gpio_auto_charge_set(DISABLE);
        SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);

        clean_fun_ctrl(IR_RECV_SEWAGE_START_CMD);
        workstation_state_var.workstation_state = DRAINING;
        workstation_state_var.flag = 1;
    } else {
        clean_fun_ctrl(IR_RECV_SEWAGE_STOP_CMD);
        workstation_state_var.workstation_state = STOPING;
    }

    res_in->success = 1;
}

static void workstation_self_clean_handle(const void *req, void *res) {
    std_srvs__srv__SetBool_Request *req_in =
        (std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response *res_in =
        (std_srvs__srv__SetBool_Response *) res;

    LOG_DEBUG("%s: [%d].\n", __FUNCTION__, req_in->data);

    if (req_in->data == 0x01) {
        drv_gpio_auto_charge_set(DISABLE);
        SYS_CLR_BIT(auto_charge_var.flag, BATTERY_UPDATE_REQ_BIT);

        clean_fun_ctrl(IR_RECV_TRANSITION_BOX_CLEAN_START_CMD);
        workstation_state_var.workstation_state = CLEANING;
    } else {
        clean_fun_ctrl(IR_RECV_TRANSITION_BOX_CLEAN_STOP_CMD);
        workstation_state_var.workstation_state = STOPING;
    }
    workstation_state_var.flag = 1;

    res_in->success = 1;
}
