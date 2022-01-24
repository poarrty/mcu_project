#include "sys_pubsub.h"
#include "cmsis_os.h"
#include "queue.h"
#include "shell.h"

#include "elog.h"

#define IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#define IS_IRQ_MODE()   (__get_IPSR() != 0U)
#define IS_IRQ()        (IS_IRQ_MODE() || (IS_IRQ_MASKED() && (osKernelGetState() == osKernelRunning)))

///< 发布总线
osEventFlagsId_t sys_pub;

///< 订阅者
osEventFlagsId_t sys_suber_paras;
osEventFlagsId_t sys_suber_led;
osEventFlagsId_t sys_suber_pmu;
osEventFlagsId_t sys_suber_display;
osEventFlagsId_t sys_suber_motor;
osEventFlagsId_t sys_suber_charge;
osEventFlagsId_t sys_suber_cliff;

osEventFlagsId_t sys_suber_uros;

///< 主题
QueueHandle_t topic_led;
QueueHandle_t topic_button;
QueueHandle_t topic_pmu_sta;
QueueHandle_t topic_pmu_ctrl;
QueueHandle_t topic_charge;
QueueHandle_t topic_bump_detect;
QueueHandle_t topic_display_ctrl;
QueueHandle_t topic_batt_sta;
QueueHandle_t topic_odom;
QueueHandle_t topic_battery;
QueueHandle_t topic_imu;
QueueHandle_t topic_cliff;
QueueHandle_t topic_pile_location;
QueueHandle_t topic_charge_feedback;
QueueHandle_t topic_encoding_l;
QueueHandle_t topic_encoding_r;
QueueHandle_t topic_motor_ctrl;

void pubsub_init(void) {
    sys_pub = osEventFlagsNew(NULL);

    sys_suber_paras   = osEventFlagsNew(NULL);
    sys_suber_led     = osEventFlagsNew(NULL);
    sys_suber_pmu     = osEventFlagsNew(NULL);
    sys_suber_display = osEventFlagsNew(NULL);
    sys_suber_motor   = osEventFlagsNew(NULL);
    sys_suber_charge  = osEventFlagsNew(NULL);
    sys_suber_cliff   = osEventFlagsNew(NULL);
    sys_suber_uros    = osEventFlagsNew(NULL);

    topic_led             = xQueueCreate(1, sizeof(LED_CTRL_T));
    topic_button          = xQueueCreate(1, sizeof(BUTTON_STA_T));
    topic_pmu_sta         = xQueueCreate(1, sizeof(PMU_STA_T));
    topic_pmu_ctrl        = xQueueCreate(1, sizeof(PMU_CTRL_T));
    topic_charge          = xQueueCreate(1, sizeof(CHARGE_CTRL_T));
    topic_bump_detect     = xQueueCreate(1, sizeof(BUMP_DETECT_T));
    topic_display_ctrl    = xQueueCreate(1, sizeof(DISPLAY_CTRL_T));
    topic_batt_sta        = xQueueCreate(1, sizeof(BATT_STA_T));
    topic_odom            = xQueueCreate(1, sizeof(nav_msgs__msg__Odometry));
    topic_battery         = xQueueCreate(1, sizeof(sensor_msgs__msg__BatteryState));
    topic_imu             = xQueueCreate(1, sizeof(sensor_msgs__msg__Imu));
    topic_cliff           = xQueueCreate(1, sizeof(sensor_msgs__msg__Range));
    topic_pile_location   = xQueueCreate(1, sizeof(std_msgs__msg__Float32));
    topic_charge_feedback = xQueueCreate(1, sizeof(std_msgs__msg__UInt8));
    topic_encoding_l      = xQueueCreate(1, sizeof(cvte_sensor_msgs__msg__Encoding));
    topic_encoding_r      = xQueueCreate(1, sizeof(cvte_sensor_msgs__msg__Encoding));
    topic_motor_ctrl      = xQueueCreate(1, sizeof(MOTOR_CTRL_T));
}

void task_fal_pubsub_run(void *argument) {
    uint32_t pub_evt;

    pubsub_init();

    for (;;) {
        ///< 检测发布到总线的主题数据
        pub_evt = osEventFlagsWait(sys_pub, SYS_EVT_ALL, osFlagsWaitAny, osWaitForever);

        ///< 将主题分发到订阅者
        if (pub_evt & SYS_EVT_PARAS_UPDATE) {
            osEventFlagsSet(sys_suber_paras, SYS_EVT_PARAS_UPDATE);
        }

        if (pub_evt & SYS_EVT_LED_CONTROL) {
            osEventFlagsSet(sys_suber_led, SYS_EVT_LED_CONTROL);
        }

        if (pub_evt & SYS_EVT_BUTTON_STA) {
            osEventFlagsSet(sys_suber_pmu, SYS_EVT_BUTTON_STA);
        }

        if (pub_evt & SYS_EVT_PMU_STA) {
            osEventFlagsSet(sys_suber_led, SYS_EVT_PMU_STA);
            osEventFlagsSet(sys_suber_display, SYS_EVT_PMU_STA);
            osEventFlagsSet(sys_suber_motor, SYS_EVT_PMU_STA);
            osEventFlagsSet(sys_suber_charge, SYS_EVT_PMU_STA);
        }

        if (pub_evt & SYS_EVT_PMU_CTRL) {
            osEventFlagsSet(sys_suber_pmu, SYS_EVT_PMU_CTRL);
        }

        if (pub_evt & SYS_EVT_AUTO_CHARGE) {
            osEventFlagsSet(sys_suber_charge, SYS_EVT_AUTO_CHARGE);
        }

        if (pub_evt & SYS_EVT_BUMP_DETECT) {
            osEventFlagsSet(sys_suber_cliff, SYS_EVT_BUMP_DETECT);
        }

        if (pub_evt & SYS_EVT_DISPLAY_CTRL) {
            osEventFlagsSet(sys_suber_display, SYS_EVT_DISPLAY_CTRL);
        }

        if (pub_evt & SYS_EVT_BATT_STA) {
            osEventFlagsSet(sys_suber_pmu, SYS_EVT_BATT_STA);
            osEventFlagsSet(sys_suber_display, SYS_EVT_BATT_STA);
        }

        if (pub_evt & SYS_EVT_ODOM) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_ODOM);
        }

        if (pub_evt & SYS_EVT_BATTERY) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_BATTERY);
        }

        if (pub_evt & SYS_EVT_IMU) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_IMU);
        }

        if (pub_evt & SYS_EVT_CLIFF) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_CLIFF);
        }

        if (pub_evt & SYS_EVT_PILE_LOCATION) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_PILE_LOCATION);
        }

        if (pub_evt & SYS_EVT_CHARGE_FEEDBACK) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_CHARGE_FEEDBACK);
        }

        if (pub_evt & SYS_EVT_ENCODING_L) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_ENCODING_L);
        }

        if (pub_evt & SYS_EVT_ENCODING_R) {
            osEventFlagsSet(sys_suber_uros, SYS_EVT_ENCODING_R);
        }

        if (pub_evt & SYS_EVT_MOTOR_CTRL) {
            osEventFlagsSet(sys_suber_motor, SYS_EVT_MOTOR_CTRL);
        }
    }
}

void pub_test(LAMP_E lamp, LED_COLOR_E color) {
    LED_CTRL_T led_ctrl;

    led_ctrl.lamp  = lamp;
    led_ctrl.color = color;

    xQueueOverwrite(topic_led, &led_ctrl);

    osEventFlagsSet(sys_pub, SYS_EVT_LED_CONTROL);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pubTest, pub_test, pubsub test);

void pub_topic(SYS_EVENT_E evt, void *data) {
    QueueHandle_t topic_queue = NULL;

    BaseType_t yield;

    switch (evt) {
        case SYS_EVT_LED_CONTROL:
            topic_queue = topic_led;
            break;

        case SYS_EVT_BUTTON_STA:
            topic_queue = topic_button;
            break;

        case SYS_EVT_PMU_STA:
            topic_queue = topic_pmu_sta;
            break;

        case SYS_EVT_PMU_CTRL:
            topic_queue = topic_pmu_ctrl;
            break;

        case SYS_EVT_AUTO_CHARGE:
            topic_queue = topic_charge;
            break;

        case SYS_EVT_BUMP_DETECT:
            topic_queue = topic_bump_detect;
            break;

        case SYS_EVT_DISPLAY_CTRL:
            topic_queue = topic_display_ctrl;
            break;

        case SYS_EVT_BATT_STA:
            topic_queue = topic_batt_sta;
            break;

        case SYS_EVT_ODOM:
            topic_queue = topic_odom;
            break;

        case SYS_EVT_BATTERY:
            topic_queue = topic_battery;
            break;

        case SYS_EVT_IMU:
            topic_queue = topic_imu;
            break;

        case SYS_EVT_CLIFF:
            topic_queue = topic_cliff;
            break;

        case SYS_EVT_PILE_LOCATION:
            topic_queue = topic_pile_location;
            break;

        case SYS_EVT_CHARGE_FEEDBACK:
            topic_queue = topic_charge_feedback;
            break;

        case SYS_EVT_ENCODING_L:
            topic_queue = topic_encoding_l;
            break;

        case SYS_EVT_ENCODING_R:
            topic_queue = topic_encoding_r;
            break;

        case SYS_EVT_MOTOR_CTRL:
            topic_queue = topic_motor_ctrl;
            break;

        default:
            break;
    }

    if (topic_queue != NULL) {
        if (IS_IRQ()) {
            xQueueOverwriteFromISR(topic_queue, data, &yield);
        } else {
            xQueueOverwrite(topic_queue, data);
        }

        osEventFlagsSet(sys_pub, evt);
    } else {
        log_e("want to public a null ipc topic!");
    }
}

uint8_t soft_button_click(void) {
    BUTTON_STA_T button_sta;

    button_sta.button = BUTTON_PWR;
    button_sta.event  = SINGLE_CLICK;
    pub_topic(SYS_EVT_BUTTON_STA, &button_sta);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), soft_click, soft_button_click, power button_click);

uint8_t pile_request(uint16_t pile_id) {
    CHARGE_CTRL_T ctrl;

    ctrl.pile_id = (CHARGE_CTRL_E) pile_id;

    pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pile_request, pile_request, pile_request);

uint8_t pmu_ctrl(uint8_t is_system_on) {
    PMU_CTRL_T pmu_ctrl;

    if (is_system_on) {
        pmu_ctrl.cmd = PMU_STA_STARTING;
    } else {
        pmu_ctrl.cmd = PMU_STA_SHUTDOWN;
    }

    pub_topic(SYS_EVT_PMU_CTRL, &pmu_ctrl);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pmu_ctrl, pmu_ctrl, pmu_ctrl);
