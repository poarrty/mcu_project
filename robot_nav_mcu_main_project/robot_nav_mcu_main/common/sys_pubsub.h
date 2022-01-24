#ifndef __SYS_EVENT_H
#define __SYS_EVENT_H
#include "cmsis_os.h"
#include "bsp_led.h"
#include "queue.h"
#include "stdbool.h"
#include "bsp_button.h"
#include "fal_pmu.h"
#include "fal_charge.h"
#include "fal_battery.h"
#include "fal_display.h"
#include "fal_motor.h"

#include <nav_msgs/msg/odometry.h>
#include <sensor_msgs/msg/battery_state.h>
#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/range.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/u_int8.h>
#include "cvte_sensor_msgs/msg/encoding.h"

#define SYS_EVT_H 0x00000001U

///< 系统事件集
typedef enum {
    SYS_EVT_PARAS_UPDATE    = SYS_EVT_H << 0,
    SYS_EVT_LED_CONTROL     = SYS_EVT_H << 1,
    SYS_EVT_BUTTON_STA      = SYS_EVT_H << 2,
    SYS_EVT_PMU_STA         = SYS_EVT_H << 3,
    SYS_EVT_PMU_CTRL        = SYS_EVT_H << 4,
    SYS_EVT_AUTO_CHARGE     = SYS_EVT_H << 5,
    SYS_EVT_BUMP_DETECT     = SYS_EVT_H << 6,
    SYS_EVT_DISPLAY_CTRL    = SYS_EVT_H << 7,
    SYS_EVT_BATT_STA        = SYS_EVT_H << 8,
    SYS_EVT_ODOM            = SYS_EVT_H << 9,
    SYS_EVT_BATTERY         = SYS_EVT_H << 10,
    SYS_EVT_IMU             = SYS_EVT_H << 11,
    SYS_EVT_CLIFF           = SYS_EVT_H << 12,
    SYS_EVT_PILE_LOCATION   = SYS_EVT_H << 13,
    SYS_EVT_CHARGE_FEEDBACK = SYS_EVT_H << 14,
    SYS_EVT_ENCODING_L      = SYS_EVT_H << 15,
    SYS_EVT_ENCODING_R      = SYS_EVT_H << 16,
    SYS_EVT_MOTOR_CTRL      = SYS_EVT_H << 17,
    SYS_EVT_ALL = SYS_EVT_PARAS_UPDATE | SYS_EVT_LED_CONTROL | SYS_EVT_BUTTON_STA | SYS_EVT_PMU_CTRL | SYS_EVT_PMU_STA | SYS_EVT_AUTO_CHARGE |
                  SYS_EVT_BUMP_DETECT | SYS_EVT_DISPLAY_CTRL | SYS_EVT_BATT_STA | SYS_EVT_ODOM | SYS_EVT_BATTERY | SYS_EVT_IMU | SYS_EVT_CLIFF |
                  SYS_EVT_PILE_LOCATION | SYS_EVT_CHARGE_FEEDBACK | SYS_EVT_ENCODING_L | SYS_EVT_ENCODING_R | SYS_EVT_MOTOR_CTRL

} SYS_EVENT_E;

///< 系统事件集
/*
typedef enum
{
    TOPIC_LED,
    TOPIC_BUTTON,
    TOPIC_PMU_STA,
    TOPIC_PMU_CTRL,
    TOPIC_AUTO_CHARGE,
    TOPIC_DUMP_DETECT,

}SYS_TOPIC_E;
*/

///< 主题：灯光控制
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    LAMP_E       lamp;
    LED_COLOR_E  color;

} LED_CTRL_T, *LED_CTRL_P;

///< 主题：按键状态
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    BUTTON_E     button;
    PressEvent   event;

} BUTTON_STA_T, *BUTTON_STA_P;

///< 主题：电源管理状态
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    PMU_STA_E    sta;

} PMU_STA_T, *PMU_STA_P;

///< 主题：电源管理控制
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    PMU_STA_E    cmd;

} PMU_CTRL_T, *PMU_CTRL_P;

///< 主题：自动充电控制
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    uint16_t     pile_id;
} CHARGE_CTRL_T, *CHARGE_CTRL_P;

///< 主题：颠簸检测
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    bool         is_bump_detected;

} BUMP_DETECT_T, *BUMP_DETECT_P;

///< 主题：显示控制
typedef struct {
    osThreadId_t  sender;
    TickType_t    ts;
    DISPLAY_CMD_T cmd;

} DISPLAY_CTRL_T, *DISPLAY_CTRL_P;

///< 主题：电池状态
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    BATT_STA_E   sta;

} BATT_STA_T, *BATT_STA_P;

///< 主题：电机控制
typedef struct {
    osThreadId_t sender;
    TickType_t   ts;
    MOTOR_CTRL_E cmd;
    uint16_t     output_current;

} MOTOR_CTRL_T, *MOTOR_CTRL_P;

extern osEventFlagsId_t sys_pub;

extern osEventFlagsId_t sys_suber_paras;
extern osEventFlagsId_t sys_suber_led;
extern osEventFlagsId_t sys_suber_pmu;
extern osEventFlagsId_t sys_suber_display;
extern osEventFlagsId_t sys_suber_motor;
extern osEventFlagsId_t sys_suber_charge;
extern osEventFlagsId_t sys_suber_cliff;
extern osEventFlagsId_t sys_suber_uros;

extern QueueHandle_t topic_led;
extern QueueHandle_t topic_button;
extern QueueHandle_t topic_pmu_sta;
extern QueueHandle_t topic_pmu_ctrl;
extern QueueHandle_t topic_charge;
extern QueueHandle_t topic_bump_detect;
extern QueueHandle_t topic_display_ctrl;
extern QueueHandle_t topic_batt_sta;
extern QueueHandle_t topic_odom;
extern QueueHandle_t topic_battery;
extern QueueHandle_t topic_imu;
extern QueueHandle_t topic_cliff;
extern QueueHandle_t topic_pile_location;
extern QueueHandle_t topic_charge_feedback;
extern QueueHandle_t topic_encoding_l;
extern QueueHandle_t topic_encoding_r;
extern QueueHandle_t topic_motor_ctrl;

void pub_topic(SYS_EVENT_E evt, void *data);

#endif
