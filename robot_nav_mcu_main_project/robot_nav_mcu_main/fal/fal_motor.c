/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-26
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
#include "fal_motor.h"
#include "cmsis_os.h"
#include "bsp_motor.h"
#include "bsp_motor_syntron.h"
#include "bsp_motor_fengdekong.h"
#include "bsp_motor_taizhao.h"
#include "shell.h"
#include "sys_pubsub.h"
#include "sys_exc.h"
#include "sys_paras.h"
#include "bsp_imu.h"
#include "math.h"
#include "fal_imu.h"
#include "typedef.h"
#include "fal_update.h"
#include "pal_uros.h"
#include "nav_msgs/msg/odometry.h"
#include "geometry_msgs/msg/twist.h"
#include "cvte_sensor_msgs/msg/encoding.h"
#include "std_msgs/msg/string.h"
#include "std_msgs/msg/u_int8.h"
#include "cJSON.h"

#define LOG_TAG "fal_motor"
#include "elog.h"

/**
 * @addtogroup Robot-FAL
 * @{
 */

/**
 * @defgroup FAL_MOTOR 功能适配层 - MOTOR
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
#define PI 3.1415926f

#define MOTOR_LIFE_EXTEND 500

//#define ODOM_PUB

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

///< 电机控制状态
typedef enum {
    MOTOR_RUN          = 0,
    MOTOR_EMERGER_STOP = 1,

} MOTOR_CONTROL_STA_E;

///< 轮胎结构相关参数
typedef struct {
    uint32_t wheel_diameter;  ///< 轮子直径/mm
    uint32_t wheel_space;     ///< 轮子间距/mm

} WHEEL_PARAS_T, *WHEEL_PARAS_P;

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
osMutexId_t mutex_motor_data = NULL;

const osMutexAttr_t mutex_motor_data_attr = {
    "mutex_motor_data",                     // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

osMutexId_t mutex_motor_data_upload = NULL;

const osMutexAttr_t mutex_motor_data_upload_attr = {
    "mutex_motor_data_upload",              // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

float    euler_yaw_rad      = 0.0;  ///< 航向角
float    euler_gyro_z_rad   = 0.0;  ///< 角速度
float    speed_mps          = 0.0;  ///< 线速度
float    odom_x_cm          = 0.0;  ///< X 轴方向位移
float    odom_y_cm          = 0.0;  ///< Y 轴方向位移
float    odom_total_cm      = 0.0;  ///< 总里程
uint16_t motor_l_error_code = 0;
uint16_t motor_r_error_code = 0;
int16_t  motor_current_l    = 0;
int16_t  motor_current_r    = 0;
int16_t  motor_real_speed_l = 0;
int16_t  motor_real_speed_r = 0;

MOTOR_POSITION_T position_t = {0};                ///< 编码器位置，自研驱动器
int32_t          pos_l_save = 0, pos_r_save = 0;  ///< 编码器位置，森创驱动器

uint8_t position_flag     = 0;
bool    motor_enable_flag = true;

extern IMU_DATA_T imu_sensor_data;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
osTimerId_t                timer_motor_control;
static MOTOR_CONTROL_STA_E control_sta         = MOTOR_EMERGER_STOP;
static uint32_t            motor_life_ts       = 0;
static uint32_t            motor_upload_status = 0;

int16_t mod_motor_speed_v = 0;
int16_t mod_motor_speed_w = 0;

int16_t speed_v_mcu = 0;
int16_t speed_w_mcu = 0;

uint16_t motor_max_current = 0;

osThreadId_t         task_motor_positoinHandle      = NULL;
const osThreadAttr_t task_motor_position_attributes = {.name       = "task_motor_position",
                                                       .priority   = (osPriority_t) osPriorityNormal,
                                                       .stack_size = 256 * 4};

#ifdef ODOM_PUB
static publisher               g_uros_odom;
static nav_msgs__msg__Odometry uros_odom = {0};
static char                    odom_buffer1[10];
static char                    odom_buffer2[10];
#endif

static publisher                       g_uros_encoding;
static cvte_sensor_msgs__msg__Encoding uros_encoding;

static int64_t encoding_buffer[2];

///< motor speed ctrl
static subscrption               g_motor_ctrl;
static geometry_msgs__msg__Twist motor_ctrl = {0};

///< motor driver ctrl
static subscrption           g_motor_driver_ctrl;
static std_msgs__msg__String motor_driver_ctrl = {0};
static char                  motor_driver_ctrl_buffer[128];

///< motor driver state
static publisher             g_motor_driver_state;
static std_msgs__msg__String motor_driver_state = {0};
static char                  motor_driver_state_buffer[128];

///< work mode
static subscrption          g_work_mode;
static std_msgs__msg__UInt8 work_mode = {0};

uint32_t motor_driver_state_pub_ts = 0;

static uint16_t motor_max_current_output;  //最大输出电流

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern osMutexId_t mutex_imu_data;
extern IMU_DATA_T  imu_sensor_data;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void    motor_control_cb(void *argument);
static uint8_t speed_translate(int16_t speed_v, int16_t speed_w, WHEEL_PARAS_T wheel, float *rpm_l, float *rpm_r);

static void task_motor_position_run(void *argument);

static void motor_position_handle(void);
static void syntron_motor_position_handle(void);
static void motor_ctrl_subscription_callback(const void *msgin);
static void motor_driver_ctrl_subscription_callback(const void *msgin);
static void fengdekong_motor_position_handle(void);
static void taizhao_motor_position_handle(void);

static void fal_motor_driver_state_public(void);

void work_mode_subscription_callback(const void *msgin);

uint16_t motor_current_sample_handle(uint16_t current_value, uint32_t *sample);
/*****************************************************************
 * 函数定义
 ******************************************************************/
typedef int (*bsp_motor_init_t)(void);
typedef uint8_t (*bsp_motor_enable_t)(void);
typedef uint8_t (*bsp_motor_disable_t)(void);
typedef uint8_t (*bsp_motor_set_speed_t)(float, float, float, MOTOR_LOCK_TYPE_E);
typedef void (*motor_position_handle_t)(void);

bsp_motor_init_t        pbsp_motor_init        = NULL;
bsp_motor_enable_t      pbsp_motor_enable      = NULL;
bsp_motor_disable_t     pbsp_motor_disable     = NULL;
bsp_motor_set_speed_t   pbsp_motor_set_speed   = NULL;
motor_position_handle_t pmotor_position_handle = NULL;

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_motor_init
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
int fal_motor_init(void) {
#ifdef ODOM_PUB
    publisher_init(&g_uros_odom, ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "/odom", &uros_odom, BEST, OVERWRITE,
                   sizeof(nav_msgs__msg__Odometry));
#endif

    publisher_init(&g_uros_encoding, ROSIDL_GET_MSG_TYPE_SUPPORT(cvte_sensor_msgs, msg, Encoding), "/encoding", &uros_encoding, BEST, OVERWRITE,
                   sizeof(cvte_sensor_msgs__msg__Encoding));

    publisher_init(&g_motor_driver_state, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/motor_driver/state", &motor_driver_state, BEST,
                   OVERWRITE, sizeof(std_msgs__msg__String));

    subscrption_init(&g_motor_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/vel_mcu", &motor_ctrl, BEST,
                     motor_ctrl_subscription_callback);

    subscrption_init(&g_motor_driver_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/motor_driver/set", &motor_driver_ctrl, BEST,
                     motor_driver_ctrl_subscription_callback);

    subscrption_init(&g_work_mode, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/clean_work_mode", &work_mode, BEST,
                     work_mode_subscription_callback);

    motor_driver_state.data.data     = motor_driver_state_buffer;
    motor_driver_state.data.size     = 0;
    motor_driver_state.data.capacity = sizeof(motor_driver_state_buffer);

    motor_driver_ctrl.data.data     = motor_driver_ctrl_buffer;
    motor_driver_ctrl.data.size     = 0;
    motor_driver_ctrl.data.capacity = sizeof(motor_driver_ctrl_buffer);

    uros_encoding.encoding_num.data     = encoding_buffer;
    uros_encoding.encoding_num.capacity = 0;
    uros_encoding.encoding_num.size     = sizeof(encoding_buffer) / sizeof(encoding_buffer[0]);

#ifdef ODOM_PUB
    uros_odom.header.frame_id.data     = odom_buffer1;
    uros_odom.header.frame_id.capacity = 10;
    uros_odom.child_frame_id.data      = odom_buffer2;
    uros_odom.child_frame_id.capacity  = 10;
    /* if not set child frame id, otherwise SOC will subcribe error. */
    sprintf(uros_odom.header.frame_id.data, "odom");
    sprintf(uros_odom.child_frame_id.data, "base_link");
#endif

    task_motor_positoinHandle = osThreadNew(task_motor_position_run, NULL, &task_motor_position_attributes);

    if (sys_paras.motor_drive_type == MOTOR_DRIVE_SELF) {
        pbsp_motor_init        = bsp_motor_init;
        pbsp_motor_set_speed   = bsp_motor_set_speed;
        pmotor_position_handle = motor_position_handle;
    }

    else if (sys_paras.motor_drive_type == MOTOR_DRIVE_SYNTRON) {
        pbsp_motor_init        = bsp_motor_syntron_init;
        pbsp_motor_enable      = bsp_syntron_motor_enable;
        pbsp_motor_disable     = Motor_Disable;
        pbsp_motor_set_speed   = bsp_motor_syntron_set_speed;
        pmotor_position_handle = syntron_motor_position_handle;
    } else if (sys_paras.motor_drive_type == MOTOR_DRIVE_FENGDEKONG) {
        pbsp_motor_init = bsp_motor_fengdekong_init;
        // pbsp_motor_disable = Motor_Disable;
        pbsp_motor_set_speed   = bsp_motor_fengdekong_set_speed;
        pmotor_position_handle = fengdekong_motor_position_handle;
    } else if (sys_paras.motor_drive_type == MOTOR_DRIVE_TAIZHAO) {
        pbsp_motor_init        = bsp_motor_taizhao_init;
        pbsp_motor_enable      = bsp_taizhao_motor_enable;
        pbsp_motor_disable     = bsp_taizhao_motor_disable;
        pbsp_motor_set_speed   = bsp_motor_taizhao_set_speed;
        pmotor_position_handle = taizhao_motor_position_handle;
    } else {
        log_e("motor drive type error");

        while (1) { /* code */
        }
    }

    motor_max_current_output = sys_paras.motor_current.motor_max_current_output;

    pbsp_motor_init();

    mutex_motor_data = osMutexNew(&mutex_motor_data_attr);

    mutex_motor_data_upload = osMutexNew(&mutex_motor_data_upload_attr);

    timer_motor_control = osTimerNew(motor_control_cb, osTimerPeriodic, NULL, NULL);

    osTimerStart(timer_motor_control, 20U);

    motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;

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
int fal_motor_deInit(void) {
    return 0;
}

void nvg_soc_live_update(void);
void motor_ctrl_subscription_callback(const void *msgin) {
    static double                    xLast = 0, zLast = 0;
    const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *) msgin;

    if (msg->linear.x != xLast || msg->angular.z != zLast) {
        xLast = msg->linear.x;
        zLast = msg->angular.z;
        log_i("/cmd_vel: v[%.2f] w[%.2f]", msg->linear.x, msg->angular.z);
    }

    motor_speed_vw_set((int) (msg->linear.x * 1000), (int) (msg->angular.z * 1000));

    nvg_soc_live_update();
}

void motor_driver_ctrl_subscription_callback(const void *msgin) {
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *) msgin;

    cJSON *cmds = NULL;
    cJSON *cmd  = NULL;

    MOTOR_CTRL_T motor_ctrl;

    if (msg->data.data == NULL) {
        return;
    }

    log_i("Get motor_driver cmds:[%s]", msg->data.data);

    cmds = cJSON_Parse(msg->data.data);

    if (cmds == NULL) {
        return;
    }

    ///< 电机使能控制指令
    cmd = cJSON_GetObjectItemCaseSensitive(cmds, "driver_enable");
    if (cJSON_IsBool(cmd)) {
        log_i("cmd enable[%d]", cJSON_IsTrue(cmd));

        if (cJSON_IsTrue(cmd)) {
            motor_ctrl.cmd = MOTOR_ENABLE;
        } else {
            motor_ctrl.cmd = MOTOR_DISABLE;
        }

        pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
    }

    ///< 电机使能控制指令
    cmd = cJSON_GetObjectItemCaseSensitive(cmds, "state_update");
    if (cJSON_IsBool(cmd)) {
        log_i("cmd update[%d]", cJSON_IsTrue(cmd));

        ///< 触发一次状态发布
        if (motor_driver_state_pub_ts == osWaitForever) {
            motor_driver_state_pub_ts = osKernelGetTickCount();
        }
    }

    cJSON_Delete(cmds);
}

void work_mode_subscription_callback(const void *msgin) {
    const std_msgs__msg__UInt8 *msg = (const std_msgs__msg__UInt8 *) msgin;

    bool set_motor_enable = false;

    MOTOR_CTRL_T motor_ctrl;

    log_i("sub clean mode:[%d], current motor enable:[%d]", msg->data, motor_enable_flag);

    if (msg->data == 0 && motor_enable_flag == true) {
        return;
    }

    if (msg->data == 1 && motor_enable_flag == false) {
        return;
    }

    set_motor_enable = !motor_enable_flag;

    if (set_motor_enable) {
        motor_ctrl.cmd = MOTOR_ENABLE;
    } else {
        motor_ctrl.cmd = MOTOR_DISABLE;
    }

    pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
}

static void syntron_motor_reinit(void) {
    osTimerStop(timer_motor_control);

    osDelay(100);

    bsp_motor_syntron_init();

    position_flag = 0;

    if (motor_enable_flag != true) {
        Motor_Disable();
        Motor_Disable();
    }

    osTimerStart(timer_motor_control, 20);
}

void task_fal_motor_run(void *argument) {
    uint32_t  sub_evt;
    PMU_STA_T pmu_sta;

    osDelay(1000);

    fal_motor_init();

    // osDelay(osWaitForever);

    uint32_t print_ts        = 0;
    uint16_t motor_status_ts = osKernelGetTickCount();

    for (;;) {
        ///< 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_motor, SYS_EVT_PMU_STA | SYS_EVT_MOTOR_CTRL, osFlagsWaitAny, 100);

        if (osKernelGetTickCount() > print_ts) {
            log_i(
                "Motor sta[%d], speed_v[0X%04X][%04d]mm, speed_w[0X%04X][%04d] "
                "state[%d], Odom: X[%.2f]cm Y[%.2f]cm Total[%.2f]cm, Curent %d "
                "%d",
                control_sta, mod_motor_speed_v, mod_motor_speed_v, mod_motor_speed_w, mod_motor_speed_w, control_sta, odom_x_cm, odom_y_cm,
                odom_total_cm, motor_current_l, motor_current_r);

            print_ts = osKernelGetTickCount() + 1000;
        }

        if ((sub_evt & osFlagsError) == 0) {
            ///< 读取主题数据
            ///< 开关机状态主题
            if (sub_evt & SYS_EVT_PMU_STA) {
                xQueuePeek(topic_pmu_sta, &pmu_sta, 100);

                if (pmu_sta.sta == PMU_STA_STARTING || pmu_sta.sta == PMU_STA_SHUTDOWN) {
                    motor_speed_vw_set(0, 0);

                    osTimerStop(timer_motor_control);

                    osDelay(100);

                    // bsp_motor_syntron_init();
                    pbsp_motor_init();

                    osTimerStart(timer_motor_control, 20U);

                    motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;

                } else if (pmu_sta.sta == PMU_STA_STANDBY || pmu_sta.sta == PMU_STA_RUNNING) {
                }
            }

            ///读取轮毂电机控制数据
            if (sub_evt & SYS_EVT_MOTOR_CTRL) {
                MOTOR_CTRL_T motor_ctrl;
                xQueuePeek(topic_motor_ctrl, &motor_ctrl, 100);

                if (motor_ctrl.cmd == MOTOR_RESET) {
                    log_i("MOTOR_RESET");
                    osTimerStop(timer_motor_control);
                    osDelay(10);
                    bsp_syntron_motor_reset();
                    sys_exc_groud_set(0, 0);
                    sys_exc_groud_set(1, 0);
                    osDelay(500);

                    bsp_motor_syntron_init();
                    osTimerStart(timer_motor_control, 20);
                } else if (motor_ctrl.cmd == MOTOR_ENABLE) {
                    log_i("MOTOR_ENABLE");
                    osTimerStop(timer_motor_control);

                    osDelay(10);

                    if (pbsp_motor_enable() == 0) {
                        if (motor_enable_flag != true) {
                            motor_enable_flag = true;

                            ///< 使能状态变化，发送一次状态
                            if (motor_driver_state_pub_ts == osWaitForever) {
                                motor_driver_state_pub_ts = osKernelGetTickCount();
                            }
                        }
                    }
                    ///< 设置失败，再进行设置
                    else {
                        pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
                    }

                    osTimerStart(timer_motor_control, 20);
                } else if (motor_ctrl.cmd == MOTOR_DISABLE) {
                    log_i("MOTOR_DISABLE");
                    osTimerStop(timer_motor_control);

                    osDelay(10);

                    if (pbsp_motor_disable() == 0) {
                        if (motor_enable_flag != false) {
                            motor_enable_flag = false;

                            ///< 使能状态变化，发送一次状态
                            if (motor_driver_state_pub_ts == osWaitForever) {
                                motor_driver_state_pub_ts = osKernelGetTickCount();
                            }
                        }
                    }
                    ///< 设置失败，再进行设置
                    else {
                        pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
                    }

                    osTimerStart(timer_motor_control, 20);
                } else if (motor_ctrl.cmd == MOTOR_MAX_CURRENT) {
                    log_i("MOTOR_MAX_CURRENT:[%d]", motor_ctrl.output_current);
                    osTimerStop(timer_motor_control);
                    osDelay(10);  ///<避免与速度指令冲突
                    motor_max_current_output = motor_ctrl.output_current;
                    bsp_syntron_motor_max_current_output(motor_ctrl.output_current);
                    osDelay(20);  ///<避免与速度指令冲突
                    osTimerStart(timer_motor_control, 20);
                }
            }
        }

        if (is_sys_danger_exc_detected() || fal_charge_get_packing_req()) {
            control_sta = MOTOR_EMERGER_STOP;
        } else {
            control_sta = MOTOR_RUN;
        }

        if (osKernelGetTickCount() > motor_life_ts && fal_pmu_get_sta() == PMU_STA_RUNNING) {
            sys_exc_set(EXC33_MOTOR_COM_TO);

            if (motor_driver_state_pub_ts == osWaitForever && sys_exc_get(EXC33_MOTOR_COM_TO)) {
                motor_driver_state_pub_ts = osKernelGetTickCount();
            }

            if (sys_paras.motor_drive_type == MOTOR_DRIVE_SELF) {
            } else {
                log_i("Motor driver connect timeout, reinit.");

                osTimerStop(timer_motor_control);

                osDelay(100);
                pbsp_motor_init();

                osTimerStart(timer_motor_control, 20);

                // motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;
                osDelay(100);
            }
        } else {
            sys_exc_clear(EXC33_MOTOR_COM_TO);
        }

        if (sys_paras.motor_drive_type == MOTOR_DRIVE_SYNTRON) {
            if (osKernelGetTickCount() > motor_status_ts && fal_pmu_get_sta() == PMU_STA_RUNNING) {
                motor_status_ts = osKernelGetTickCount() + 1000;

                if (motor_upload_status != MOTOR_READY) {
                    log_e("motor_upload_status[%x]\r\n", motor_upload_status);
                    syntron_motor_reinit();
                }

                motor_upload_status = 0;
            }
        }

        if (osKernelGetTickCount() >= motor_driver_state_pub_ts) {
            fal_motor_driver_state_public();

            ///< 发生异常则以 0.5 Hz 频率上报
            if (motor_l_error_code != 0 || motor_r_error_code != 0 || sys_exc_get(EXC33_MOTOR_COM_TO)) {
                motor_driver_state_pub_ts = osKernelGetTickCount() + 2000;
            } else {
                motor_driver_state_pub_ts = osWaitForever;
            }
        }
    }
}

static void task_motor_position_run(void *argument) {
    for (;;) {
        osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever);

        while (HAL_CAN_GetRxFifoFillLevel(&CAN_MOTOR_H, CAN_RX_FIFO0) != 0) {
            if (pmotor_position_handle != NULL) {
                pmotor_position_handle();
            }
        }

        HAL_CAN_ActivateNotification(&CAN_MOTOR_H,
                                     CAN_IT_RX_FIFO0_MSG_PENDING);  ///< 接收处理完成，恢复接收中断

        // osDelay(10);
    }
}

///< 10230   60 rpm
///< speed_r rpm
uint8_t motor_speed_rpm(int16_t speed_l, int16_t speed_r, MOTOR_CONTROL_STA_E sta) {
    // speed_l;
    // speed_r;

    control_sta = sta;

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motor_speed_rpm, motor_speed_rpm, motor_speed_rpm);

///< speed_v mm/s
///< speed_w mrad/s
uint8_t motor_speed_vw_set(int16_t speed_v, int16_t speed_w) {
    int16_t speed_min, speed_max;

    speed_min = -1 * (int32_t) sys_paras.motor_mmps_max;
    speed_max = 1 * (int32_t) sys_paras.motor_mmps_max;

    ///< shell 输入限速
    speed_v = (speed_v > speed_max) ? (speed_max) : (speed_v);
    speed_v = (speed_v < speed_min) ? (speed_min) : (speed_v);

    speed_w = (speed_w > speed_max) ? (speed_max) : (speed_w);
    speed_w = (speed_w < speed_min) ? (speed_min) : (speed_w);

    osMutexAcquire(mutex_motor_data, osWaitForever);

    mod_motor_speed_v = speed_v;
    mod_motor_speed_w = speed_w;

    osMutexRelease(mutex_motor_data);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motor_speed_vw, motor_speed_vw_set, motor_speed_vw_set);

uint8_t motor_speed_vw_get(int16_t *speed_v, int16_t *speed_w) {
    osMutexAcquire(mutex_motor_data, osWaitForever);

    *speed_v = mod_motor_speed_v;
    *speed_w = mod_motor_speed_w;

    osMutexRelease(mutex_motor_data);

    return 0;
}

///< speed_v mm/s
///< speed_w mm/s
static uint8_t mcu_motor_speed_vw(int16_t speed_v, int16_t speed_w) {
    int16_t speed_min, speed_max;

    speed_min = -1 * (int32_t) sys_paras.motor_mmps_max;
    speed_max = 1 * (int32_t) sys_paras.motor_mmps_max;

    ///< shell 输入限速
    speed_v = (speed_v > speed_max) ? (speed_max) : (speed_v);
    speed_v = (speed_v < speed_min) ? (speed_min) : (speed_v);

    speed_w = (speed_w > speed_max) ? (speed_max) : (speed_w);
    speed_w = (speed_w < speed_min) ? (speed_min) : (speed_w);

    speed_v_mcu = speed_v;
    speed_w_mcu = speed_w;

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), mcu_motor_speed_vw, mcu_motor_speed_vw, mcu_motor_speed_vw);

static void motor_control_cb(void *argument) {
    static int16_t  speed_v_set = 0, speed_w_set = 0;
    static int16_t  speed_v_adj = 0, speed_w_adj = 0;
    WHEEL_PARAS_T   wheels;
    static uint32_t normal_lock_ts = 0;

    static float robot_pitch = 0.0;

    float             speed_set_l = 0, speed_set_r = 0;
    MOTOR_LOCK_TYPE_E lock;

    if (speed_v_mcu != 0 || speed_w_mcu != 0) {
        speed_v_set = speed_v_mcu;
        speed_w_set = speed_w_mcu;

    } else {
        motor_speed_vw_get(&speed_v_set, &speed_w_set);
    }

    if (osMutexAcquire(mutex_imu_data, 0) == osOK) {
        robot_pitch = imu_sensor_data.euler_pitch;
        osMutexRelease(mutex_imu_data);
    }

    ///< 自研驱动器加速度调节
    if (sys_paras.motor_drive_type == MOTOR_DRIVE_SELF) {
        int16_t motor_acc;

        if (speed_v_set == 0) {
            motor_acc = sys_paras.motor_acc_down;
        } else {
            motor_acc = sys_paras.motor_acc_up;
        }

        if (speed_v_adj < speed_v_set) {
            speed_v_adj = ((speed_v_adj + motor_acc) > speed_v_set) ? speed_v_set : (speed_v_adj + motor_acc);
        } else if (speed_v_adj > speed_v_set) {
            speed_v_adj = ((speed_v_adj - motor_acc) < speed_v_set) ? speed_v_set : (speed_v_adj - motor_acc);
        }

        if (speed_w_set == 0) {
            motor_acc = sys_paras.motor_acc_down;
        } else {
            motor_acc = sys_paras.motor_acc_up;
        }

        if (speed_w_adj < speed_w_set) {
            speed_w_adj = ((speed_w_adj + motor_acc) > speed_w_set) ? speed_w_set : (speed_w_adj + motor_acc);
        } else if (speed_w_adj > speed_w_set) {
            speed_w_adj = ((speed_w_adj - motor_acc) < speed_w_set) ? speed_w_set : (speed_w_adj - motor_acc);
        }

        ///< 机器人在斜坡，等速降为0再刹车
        /*
        if(speed_v_set == 0 && speed_w_set == 0 && (fabs(robot_pitch)*10 >=
        sys_paras.imu_pitch_threshold))
        {
            speed_v_adj = 0;
            speed_w_adj = 0;
        }
        */

    }
    ///< 森创驱动器加速度由驱动器调节
    else {
        speed_v_adj = speed_v_set;
        speed_w_adj = speed_w_set;
    }

    ///< 加速度调节

    ///< 刹车控制
    if (speed_v_adj == 0 && speed_w_adj == 0) {
        if (sys_paras.epb_delay_ms == 0 || (fabs(robot_pitch) * 10 >= sys_paras.imu_pitch_threshold)) {
            normal_lock_ts = osKernelGetTickCount();
        }
    } else {
        if (sys_paras.epb_delay_ms == 0) {
            normal_lock_ts = osKernelGetTickCount() + 1000;
        } else {
            normal_lock_ts = osKernelGetTickCount() + sys_paras.epb_delay_ms;
        }
    }

    switch (control_sta) {
        case MOTOR_RUN:
            wheels.wheel_diameter = sys_paras.wheel_diameter;
            wheels.wheel_space    = sys_paras.wheel_space;
            speed_translate(speed_v_adj, speed_w_adj, wheels, &speed_set_l, &speed_set_r);
            static float speed_set_l_last = 0;
            static float speed_set_r_last = 0;

            if (speed_set_l != speed_set_l_last || speed_set_r != speed_set_r_last) {
                speed_set_l_last = speed_set_l;
                speed_set_r_last = speed_set_r;

                log_d("/speed: l[%f] r[%f],v=%d, w=%d", speed_set_l, speed_set_r, speed_v_adj, speed_w_adj);
            }

            if (osKernelGetTickCount() >= normal_lock_ts) {
                lock = NORMAL_LOCK;
            } else {
                lock = NO_LOCK;
            }
            break;

        case MOTOR_EMERGER_STOP:
            speed_v_adj = 0;
            speed_w_adj = 0;
            speed_set_l = 0;
            speed_set_r = 0;
            lock        = EMERGER_LOCK;
            break;

        default:
            speed_v_adj = 0;
            speed_w_adj = 0;
            speed_set_l = 0;
            speed_set_r = 0;
            lock        = EMERGER_LOCK;
            break;
    }

    if (pbsp_motor_set_speed != NULL) {
        ///< 四舍五入
        if (speed_set_l > 0) {
            speed_set_l += 0.5f;
        } else if (speed_set_l < 0) {
            speed_set_l -= 0.5f;
        }

        if (speed_set_r > 0) {
            speed_set_r += 0.5f;
        } else if (speed_set_r < 0) {
            speed_set_r -= 0.5f;
        }

        pbsp_motor_set_speed(speed_set_l, speed_set_r, 1.0 * sys_paras.motor_reduction_ratio / 10, lock);
    }
}

void motor_data_handle(CAN_HandleTypeDef *hcan) {
    if (task_motor_positoinHandle != NULL) {
        osThreadFlagsSet(task_motor_positoinHandle, 0x0001U);
        HAL_CAN_DeactivateNotification(&CAN_MOTOR_H, CAN_IT_RX_FIFO0_MSG_PENDING);
    }
}

static void motor_position_handle(void) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    MOTOR_POSITION_P position_p;
    // MOTOR_ERROR_BITS_P error_p;
    MOTOR_SPEED_P speed_p;

#ifdef ODOM_PUB
    static uint32_t ts = 0;
    int16_t         position_delt_l, position_delt_r;
    double          position_delt;
    float           distance;
    uint32_t        time_inval;
#endif

    uint16_t motor_errs = 0;

    // uint32_t offset = 0;
    // uint16_t size = 0;

    if (HAL_CAN_GetRxMessage(&CAN_MOTOR_H, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        switch (RxHeader.ExtId) {
            case CAN_ID_POSITION:
                // elog_hexdump("Motor position state", 8, RxData, 8);
                motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;

                position_p = (MOTOR_POSITION_P) RxData;

                log_d("POS_L[%05d] POS_R[%05d]", position_p->position_l, position_p->position_r);

                if (position_flag == 0) {
                    position_t = *position_p;
#ifdef ODOM_PUB
                    ts = HAL_GetTick();
#endif
                    position_flag = 1;
                } else {
#ifdef ODOM_PUB
                    ///< 获取移动距离
                    ///< 因为用int16_t 有符号数存储结果，不管正向越过
                    ///< 65535还是反向，存储结果都是正确的。
                    position_delt_l = position_p->position_l - position_t.position_l;
                    position_delt_r = position_p->position_r - position_t.position_r;

                    ///< 自研电机适配
                    if (sys_paras.motor_inc_per_round == 16384) {
                        ///< 越界处理
                        position_delt_l = (position_delt_l <= -1 * (int32_t) sys_paras.motor_inc_per_round / 2)
                                              ? (position_delt_l + sys_paras.motor_inc_per_round)
                                              : position_delt_l;
                        position_delt_l = (position_delt_l >= 1 * (int32_t) sys_paras.motor_inc_per_round / 2)
                                              ? (position_delt_l - sys_paras.motor_inc_per_round)
                                              : position_delt_l;

                        position_delt_r = (position_delt_r <= -1 * (int32_t) sys_paras.motor_inc_per_round / 2)
                                              ? (position_delt_r + sys_paras.motor_inc_per_round)
                                              : position_delt_r;
                        position_delt_r = (position_delt_r >= 1 * (int32_t) sys_paras.motor_inc_per_round / 2)
                                              ? (position_delt_r - sys_paras.motor_inc_per_round)
                                              : position_delt_r;
                    }

                    position_delt = (position_delt_l + position_delt_r) / 2.0;
                    distance =
                        PI * sys_paras.wheel_diameter * (1.0 * position_delt / sys_paras.motor_inc_per_round) / 10;  ///< 距离计算, 毫米转厘米

                    ///< 获取时间间隔
                    time_inval = HAL_GetTick() - ts;
                    ts         = HAL_GetTick();

                    ///< 获取航向角和角速度
                    osMutexAcquire(mutex_imu_data, osWaitForever);
                    euler_yaw_rad    = imu_sensor_data.euler_yaw * PI / 180.0f;
                    euler_gyro_z_rad = imu_sensor_data.gyro_z_adj_offset * PI / 180.0f;
                    osMutexRelease(mutex_imu_data);

                    ///< 计算线速度、位移、总里程
                    speed_mps = distance / 100 / (1.0 * time_inval / 1000);
                    odom_x_cm += distance * cosf(euler_yaw_rad);
                    odom_y_cm += distance * sinf(euler_yaw_rad);
                    odom_total_cm += fabs(distance);

                    position_t = *position_p;  ///< 已更新里程，可以载入新的位置信息

                    log_d("POS_L[%05d] POS_R[%05d] [%05d] [%.6f] [%.6f]", position_p->position_l, position_p->position_r, position_delt,
                          distance, odom_total_cm);

                    // odom data pub
                    uros_odom.pose.pose.position.x  = odom_x_cm / 100;
                    uros_odom.pose.pose.position.y  = odom_y_cm / 100;
                    uros_odom.pose.pose.position.z  = 0;
                    uros_odom.twist.twist.linear.x  = speed_mps;
                    uros_odom.twist.twist.linear.y  = 0.0;
                    uros_odom.twist.twist.linear.z  = 0.0;
                    uros_odom.twist.twist.angular.x = 0.0;
                    uros_odom.twist.twist.angular.y = 0.0;
                    uros_odom.twist.twist.angular.z = euler_gyro_z_rad;

                    osMutexAcquire(mutex_imu_data, osWaitForever);

                    uros_odom.pose.pose.orientation.w = imu_sensor_data.quat_w;
                    uros_odom.pose.pose.orientation.x = imu_sensor_data.quat_x * (-1.0);
                    uros_odom.pose.pose.orientation.y = imu_sensor_data.quat_y * (-1.0);
                    uros_odom.pose.pose.orientation.z = imu_sensor_data.quat_z * (-1.0);

                    osMutexRelease(mutex_imu_data);

                    pal_uros_msg_set_timestamp(&uros_odom.header.stamp);

                    log_d("Motor pub odom");
                    message_publish(&g_uros_odom);

#endif

                    uros_encoding.encoding_num.data[0] = position_p->position_l;
                    uros_encoding.encoding_circle      = 600;
                    uros_encoding.encoding_num.data[1] = position_p->position_r;
                    uros_encoding.encoding_circle      = 600;

                    pal_uros_msg_set_timestamp(&uros_encoding.header.stamp);

                    message_publish(&g_uros_encoding);
                }

                break;

            case CAN_ID_ERROR_STA:
                // elog_hexdump("Motor state", 8, RxData, 8);

                /*
                error_p = (MOTOR_ERROR_BITS_P)RxData;

                if(error_p->lock_l)
                    sys_exc_set(EXC05_MOTOR1L_LOCK);
                else
                    sys_exc_clear(EXC05_MOTOR1L_LOCK);

                if(error_p->lock_r)
                    sys_exc_set(EXC06_MOTOR1R_LOCK);
                else
                    sys_exc_clear(EXC06_MOTOR1R_LOCK);
                */

                motor_errs = RxData[1];
                motor_errs <<= 8;
                motor_errs |= RxData[0];

                sys_exc_groud_set(0, motor_errs);

                speed_p = (MOTOR_SPEED_P) RxData;

                motor_current_l = speed_p->current_l;
                motor_current_r = speed_p->current_r;

                break;
                /*
                case CAN_ID_UPDATE_HANDSHAKE_REQ:

                    osEventFlagsSet(evt_motor_update,
                                    EVENT_GROUP_UPDATE_MOTOR_HEADSHAKE);

                    break;

                case CAN_ID_UPDATE_PACK_REQ:

                    offset = RxData[0];
                    offset <<= 8;
                    offset |= RxData[1];
                    offset <<= 8;
                    offset |= RxData[2];
                    offset <<= 8;
                    offset |= RxData[3];

                    size = RxData[4];
                    size <<= 8;
                    size |= RxData[5];

                    fal_update_set_motor_pack_trf(offset, size);

                    osEventFlagsSet(evt_motor_update,
                                    EVENT_GROUP_UPDATE_MOTOR_PACK_TRF);

                    break;

                case CAN_ID_UPDATE_FINISH_REQ:

                    osEventFlagsSet(evt_motor_update,
                                    EVENT_GROUP_UPDATE_MOTOR_CHECK);

                    break;

                case CAN_ID_UPDATE_FAILED:

                    osEventFlagsSet(evt_motor_update,
                                    EVENT_GROUP_UPDATE_MOTOR_FAILED);

                    break;

                case CAN_ID_UPDATE_SUCCESS:

                    osEventFlagsSet(evt_motor_update,
                                    EVENT_GROUP_UPDATE_MOTOR_SUCCESS);

                    break;
                */

            default:
                break;
        }

    } else {
    }
}

static void syntron_motor_position_handle(void) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    // MOTOR_POSITION_P position_p;
    // MOTOR_ERROR_BITS_P error_p;

#ifdef ODOM_PUB
    static uint32_t ts = 0;
    int16_t         position_delt_l, position_delt_r;
    double          position_delt;
    float           distance;
    uint32_t        time_inval;
#endif

    static int32_t pos_l_temp = 0;

    int32_t pos_r;

    static uint8_t start_flag = 0;

    uint16_t motor_load_rate = 0;

    static uint32_t left_current_sample  = 0;
    static uint32_t right_current_sample = 0;
    MOTOR_CTRL_T    motor_ctrl;
    uint16_t        output_current;

    if (HAL_CAN_GetRxMessage(&SYNTRON_CAN_MOTOR_H, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        switch (RxHeader.StdId) {
            case SYNTRON_CAN_ID_L:
            case SYNTRON_CAN_ID_R:

                set_syntron_msg(RxData);
                break;

            case SYNTRON_CAN_MOTOR_R_STATUS:
                motor_r_error_code = (RxData[1] << 8) + RxData[0];
                motor_load_rate    = (RxData[3] << 8) + RxData[2];

                ///<电机额定电流20A，motor_load_rate为电流千分比，motor_current_r单位0.1A
                motor_current_r = motor_load_rate * 20 / 100;
                if (motor_r_error_code == 0) {
                    sys_exc_groud_set(1, 0);
                } else {
                    sys_exc_groud_set(1, 1 << (motor_r_error_code - 1));
                }

                output_current = motor_current_sample_handle(motor_current_r, &right_current_sample);

                if (output_current != 0) {
                    motor_ctrl.cmd            = MOTOR_MAX_CURRENT;
                    motor_ctrl.output_current = output_current;
                    pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
                }

                SYS_SET_BIT(motor_upload_status, MOTOR_ERROR_CODE_R);
                SYS_SET_BIT(motor_upload_status, MOTOR_CURRENT_R);

                break;
            case SYNTRON_CAN_MOTOR_L_STATUS:
                motor_l_error_code = (RxData[1] << 8) + RxData[0];
                motor_load_rate    = (RxData[3] << 8) + RxData[2];
                ///<电机额定电流20A，motor_load_rate为电流千分比，motor_current_l单位0.1A
                motor_current_l = motor_load_rate * 20 / 100;

                output_current = motor_current_sample_handle(motor_current_l, &left_current_sample);

                if (output_current != 0) {
                    motor_ctrl.cmd            = MOTOR_MAX_CURRENT;
                    motor_ctrl.output_current = output_current;
                    pub_topic(SYS_EVT_MOTOR_CTRL, &motor_ctrl);
                }

                if (motor_l_error_code == 0) {
                    sys_exc_groud_set(0, 0);
                } else {
                    sys_exc_groud_set(0, 1 << (motor_l_error_code - 1));
                }

                SYS_SET_BIT(motor_upload_status, MOTOR_ERROR_CODE_L);
                SYS_SET_BIT(motor_upload_status, MOTOR_CURRENT_L);

                break;

            case SYNTRON_CAN_ID_L_POS:

                // elog_hexdump("Motor position state", 8, RxData, 8);
                if (RxHeader.DLC == 8) {
                    pos_l_temp =
                        ((int32_t) RxData[0] << 0) + ((int32_t) RxData[1] << 8) + ((int32_t) RxData[2] << 16) + ((int32_t) RxData[3] << 24);
                    motor_real_speed_l =
                        (((int32_t) RxData[4] << 0) + ((int32_t) RxData[5] << 8) + ((int32_t) RxData[6] << 16) + ((int32_t) RxData[7] << 24)) *
                        0.1 / 60 * PI * sys_paras.wheel_diameter;

                    start_flag = 1;
                    // log_d("SYNTRON POS_L[%ld]", pos);

                    SYS_SET_BIT(motor_upload_status, MOTOR_ENCODER_L);
                    SYS_SET_BIT(motor_upload_status, MOTOR_SPEED_L);
                }

                break;

            case SYNTRON_CAN_ID_R_POS:

                // elog_hexdump("Motor position state", 8, RxData, 8);

                if (RxHeader.DLC == 8) {
                    pos_r = ((int32_t) RxData[0] << 0) + ((int32_t) RxData[1] << 8) + ((int32_t) RxData[2] << 16) + ((int32_t) RxData[3] << 24);
                    motor_real_speed_r =
                        (((int32_t) RxData[4] << 0) + ((int32_t) RxData[5] << 8) + ((int32_t) RxData[6] << 16) + ((int32_t) RxData[7] << 24)) *
                        0.1 / 60 * PI * sys_paras.wheel_diameter;

                    // log_d("SYNTRON POS_R[%ld]", pos_r);

                    if (position_flag == 0) {
                        pos_l_save = pos_l_temp;
                        pos_r_save = pos_r;

                        if (start_flag == 1) {
                            position_flag = 1;
                        }
                    } else {
#ifdef ODOM_PUB
                        ///< 获取移动距离
                        position_delt_l = pos_l_temp - pos_l_save;
                        position_delt_r = pos_r_save - pos_r;
                        position_delt   = (position_delt_l + position_delt_r) / 2.0;
                        distance        = PI * sys_paras.wheel_diameter * (1.0 * position_delt / sys_paras.motor_inc_per_round) /
                                   10;  ///< 距离计算, 毫米转厘米

                        ///< 获取时间间隔
                        time_inval = HAL_GetTick() - ts;
                        ts         = HAL_GetTick();

                        ///< 获取航向角和角速度
                        osMutexAcquire(mutex_imu_data, osWaitForever);
                        euler_yaw_rad    = imu_sensor_data.euler_yaw * PI / 180.0f;
                        euler_gyro_z_rad = imu_sensor_data.gyro_z_adj_offset * PI / 180.0f;
                        osMutexRelease(mutex_imu_data);

                        ///< 计算线速度、位移、总里程
                        speed_mps = distance / 100 / (1.0 * time_inval / 1000);
                        odom_x_cm += distance * cosf(euler_yaw_rad);
                        odom_y_cm += distance * sinf(euler_yaw_rad);
                        odom_total_cm += fabs(distance);

                        uros_odom.pose.pose.position.x  = odom_x_cm / 100;
                        uros_odom.pose.pose.position.y  = odom_y_cm / 100;
                        uros_odom.pose.pose.position.z  = 0;
                        uros_odom.twist.twist.linear.x  = speed_mps;
                        uros_odom.twist.twist.linear.y  = 0.0;
                        uros_odom.twist.twist.linear.z  = 0.0;
                        uros_odom.twist.twist.angular.x = 0.0;
                        uros_odom.twist.twist.angular.y = 0.0;
                        uros_odom.twist.twist.angular.z = euler_gyro_z_rad;

                        osMutexAcquire(mutex_imu_data, osWaitForever);

                        uros_odom.pose.pose.orientation.w = imu_sensor_data.quat_w;
                        uros_odom.pose.pose.orientation.x = imu_sensor_data.quat_x * (-1.0);
                        uros_odom.pose.pose.orientation.y = imu_sensor_data.quat_y * (-1.0);
                        uros_odom.pose.pose.orientation.z = imu_sensor_data.quat_z * (-1.0);

                        osMutexRelease(mutex_imu_data);

                        pal_uros_msg_set_timestamp(&uros_odom.header.stamp);

                        // odom data pub
                        message_publish(&g_uros_odom);
#endif

                        ///< 已更新里程，可以载入新的位置信息
                        pos_l_save = pos_l_temp;
                        pos_r_save = pos_r;

                        uros_encoding.encoding_num.data[0] = pos_l_save;
                        uros_encoding.encoding_circle      = 5600;
                        //< 向前行使，森创驱动器左右轮编码器数值反向变化
                        uros_encoding.encoding_num.data[1] = -1 * pos_r_save;
                        uros_encoding.encoding_circle      = 5600;

                        pal_uros_msg_set_timestamp(&uros_encoding.header.stamp);
                        message_publish(&g_uros_encoding);
                    }

                    SYS_SET_BIT(motor_upload_status, MOTOR_ENCODER_R);
                    SYS_SET_BIT(motor_upload_status, MOTOR_SPEED_R);
                }
                break;
            case SYNTRON_CAN_ID_HEARTBEAT:
                if (RxData[0] == 5) {
                    motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;

                } else {
                    log_e("heartbeat[%d]", RxData[0]);
                }

                break;

            default:
                break;
        }
    } else {
    }
}

static void fengdekong_motor_position_handle(void) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    static uint8_t position_flag = 0;

#ifdef ODOM_PUB
    static uint32_t ts = 0;
    int16_t         position_delt_l, position_delt_r;
    double          position_delt;
    float           distance;
    uint32_t        time_inval;
#endif

    static int32_t pos_l_temp = 0;

    int32_t pos_r;

    if (HAL_CAN_GetRxMessage(&SYNTRON_CAN_MOTOR_H, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        if (RxHeader.StdId == 0x281) {
            motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;

            pos_l_temp = ((int32_t) RxData[0] << 0) + ((int32_t) RxData[1] << 8) + ((int32_t) RxData[2] << 16) + ((int32_t) RxData[3] << 24);
            pos_r      = ((int32_t) RxData[4] << 0) + ((int32_t) RxData[5] << 8) + ((int32_t) RxData[6] << 16) + ((int32_t) RxData[7] << 24);

            if (position_flag == 1) {
#ifdef ODOM_PUB
                ///< 获取移动距离
                position_delt_l = pos_l_save - pos_l_temp;
                position_delt_r = pos_r - pos_r_save;

                position_delt = (position_delt_l + position_delt_r) / 2.0;
                distance =
                    PI * sys_paras.wheel_diameter * (1.0 * position_delt / sys_paras.motor_inc_per_round) / 10;  ///< 距离计算, 毫米转厘米

                ///< 获取时间间隔
                time_inval = HAL_GetTick() - ts;
                ts         = HAL_GetTick();

                ///< 获取航向角和角速度
                osMutexAcquire(mutex_imu_data, osWaitForever);
                euler_yaw_rad    = imu_sensor_data.euler_yaw * PI / 180.0f;
                euler_gyro_z_rad = imu_sensor_data.gyro_z_adj_offset * PI / 180.0f;
                osMutexRelease(mutex_imu_data);

                ///< 计算线速度、位移、总里程
                speed_mps = distance / 100 / (1.0 * time_inval / 1000);
                odom_x_cm += distance * cosf(euler_yaw_rad);
                odom_y_cm += distance * sinf(euler_yaw_rad);
                odom_total_cm += fabs(distance);

                ///< 已更新里程，可以载入新的位置信息
                pos_l_save = pos_l_temp;
                pos_r_save = pos_r;

                // odom data pub

                uros_odom.pose.pose.position.x  = odom_x_cm / 100;
                uros_odom.pose.pose.position.y  = odom_y_cm / 100;
                uros_odom.pose.pose.position.z  = 0;
                uros_odom.twist.twist.linear.x  = speed_mps;
                uros_odom.twist.twist.linear.y  = 0.0;
                uros_odom.twist.twist.linear.z  = 0.0;
                uros_odom.twist.twist.angular.x = 0.0;
                uros_odom.twist.twist.angular.y = 0.0;
                uros_odom.twist.twist.angular.z = euler_gyro_z_rad;

                osMutexAcquire(mutex_imu_data, osWaitForever);

                uros_odom.pose.pose.orientation.w = imu_sensor_data.quat_w;
                uros_odom.pose.pose.orientation.x = imu_sensor_data.quat_x * (-1.0);
                uros_odom.pose.pose.orientation.y = imu_sensor_data.quat_y * (-1.0);
                uros_odom.pose.pose.orientation.z = imu_sensor_data.quat_z * (-1.0);

                osMutexRelease(mutex_imu_data);

                pal_uros_msg_set_timestamp(&uros_odom.header.stamp);

                message_publish(&g_uros_odom);
#endif

                uros_encoding.encoding_num.data[0] = pos_l_save;
                uros_encoding.encoding_circle      = 4096;
                uros_encoding.encoding_num.data[1] = pos_r_save;
                uros_encoding.encoding_circle      = 4096;

                pal_uros_msg_set_timestamp(&uros_encoding.header.stamp);
                message_publish(&g_uros_encoding);

            } else {
                pos_l_save = pos_l_temp;
                pos_r_save = pos_r;

                position_flag = 1;
            }
        }
    }
}

static void taizhao_motor_position_handle(void) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    int            pos_r       = 0;
    static int     pos_l       = 0;
    MOTOR_TPDO1_T *motor_tpdo1 = NULL;
    MOTOR_TPDO2_T *motor_tpdo2 = NULL;
    MOTOR_TPDO3_T *motor_tpdo3 = NULL;

    if (HAL_CAN_GetRxMessage(&CAN_MOTOR_H, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        switch (RxHeader.StdId) {
            case COBID_18L:

                motor_tpdo1        = (MOTOR_TPDO1_T *) RxData;
                motor_l_error_code = motor_tpdo1->error_code;
                break;
            case COBID_18R:
                motor_tpdo1        = (MOTOR_TPDO1_T *) RxData;
                motor_r_error_code = motor_tpdo1->error_code;

                log_d("motor_error_code[%d][%d]", motor_r_error_code, motor_l_error_code);
                break;
            case COBID_28L:
                motor_tpdo2        = (MOTOR_TPDO2_T *) RxData;
                motor_real_speed_l = motor_tpdo2->speed;
                pos_l              = motor_tpdo2->position;

                break;
            case COBID_28R:
                motor_tpdo2        = (MOTOR_TPDO2_T *) RxData;
                motor_real_speed_r = motor_tpdo2->speed;
                pos_r              = motor_tpdo2->position;

                log_d("TAIZHAO POS_L[%ld],POS_R[%ld],speed L[%d][%d]", pos_l, pos_r, motor_real_speed_l, motor_real_speed_r);

                if (position_flag == 0) {
                    osMutexAcquire(mutex_motor_data_upload, osWaitForever);

                    pos_l_save = pos_l;
                    pos_r_save = pos_r;

                    osMutexRelease(mutex_motor_data_upload);

                    position_flag = 1;
                } else {
                    ///< 已更新里程，可以载入新的位置信息
                    pos_l_save = pos_l;
                    pos_r_save = pos_r;

                    uros_encoding.encoding_num.data[0] = pos_l_save;
                    uros_encoding.encoding_circle      = 4096;
                    //< 向前行使，森创驱动器左右轮编码器数值反向变化
                    uros_encoding.encoding_num.data[1] = -1 * pos_r_save;
                    uros_encoding.encoding_circle      = 4096;

                    pal_uros_msg_set_timestamp(&uros_encoding.header.stamp);
                    message_publish(&g_uros_encoding);
                }
                break;
            case COBID_38L:
                motor_tpdo3     = (MOTOR_TPDO3_T *) RxData;
                motor_current_l = motor_tpdo3->current;
                break;
            case COBID_38R:
                motor_tpdo3     = (MOTOR_TPDO3_T *) RxData;
                motor_current_r = motor_tpdo3->current;

                log_d("motor_current[%d][%d]", motor_current_l, motor_current_r);
                break;
            case COBID_SDO_RESP_L:
            case COBID_SDO_RESP_R:
                taizhao_sdo_resp_msg(RxData);
                break;
            case 0x701:
            case 0x702:
                motor_life_ts = osKernelGetTickCount() + MOTOR_LIFE_EXTEND;
                break;

            default:
                break;
        }

    } else {
    }
}
/*****************************************************************/
/**
 * Function:       speed_translate
 * Description:    速度转换，将线速度和角速度转换为下发给电机的 rpm(圈/分钟)
 * Calls:
 * Called By:
 * @param[in] speed_v 线速度 mm/s
 * @param[in] speed_w 角速度 mrad/s
 * @param[in] wheel 车轮结构参数
 * @param[out] rpm_l 左轮转度 0.1 rpm
 * @param[out] rpm_r 右轮转度 0.1 rpm
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
static uint8_t speed_translate(int16_t speed_v, int16_t speed_w, WHEEL_PARAS_T wheel, float *rpm_l, float *rpm_r) {
    float rpm_unit = 1.0f * 2 * PI * wheel.wheel_diameter / 2 / 60;  ///< 2*PI*R/60 --> 1 rpm 对应的 unit mm/s
    float rpm_v    = speed_v / rpm_unit;                             ///< 直行速度
    float fspeed_w = speed_w;
    float rpm_turn = fspeed_w * wheel.wheel_space / (2000 * rpm_unit);

    ///<单位0.1rpm
    *rpm_l = rpm_v - rpm_turn;
    *rpm_r = rpm_v + rpm_turn;

    return 0;
}

static void fal_motor_driver_state_public(void) {
    cJSON *pJsonRoot = NULL;
    char * lpJsonStr = NULL;

    pJsonRoot = cJSON_CreateObject();

    if (pJsonRoot != NULL) {
        if (sys_exc_get(EXC33_MOTOR_COM_TO)) {
            cJSON_AddFalseToObject(pJsonRoot, "driver_comm");
        } else {
            cJSON_AddTrueToObject(pJsonRoot, "driver_comm");
        }

        cJSON_AddBoolToObject(pJsonRoot, "driver_enable", motor_enable_flag);
        cJSON_AddNumberToObject(pJsonRoot, "motor_current_l", motor_current_l);
        cJSON_AddNumberToObject(pJsonRoot, "motor_current_r", motor_current_r);
        cJSON_AddNumberToObject(pJsonRoot, "motor_error_l", motor_l_error_code);
        cJSON_AddNumberToObject(pJsonRoot, "motor_error_r", motor_r_error_code);

        lpJsonStr = cJSON_PrintUnformatted(pJsonRoot);

        log_i("drvier state:%s", lpJsonStr);

        cJSON_Delete(pJsonRoot);

        ///< 状态 json 字符串发布
        if (lpJsonStr != NULL && strlen(lpJsonStr) <= sizeof(motor_driver_state_buffer)) {
            strcpy(motor_driver_state.data.data, lpJsonStr);
            motor_driver_state.data.size = strlen(motor_driver_state.data.data);
            message_publish(&g_motor_driver_state);

            vPortFree(lpJsonStr);
        }
    }
}

/*
 * Function:       motor_current_sample_handle
 * Description:    轮毂电机电流采样处理
 * Calls:
 * Called By:
 * @param[in]      @current_value:电流值，0.1A
 *                 @sample：采样记录
 *  * @return
 *  - 轮毂电机最大电流输出值
 *  - 0表示不变
 *  -
 */
uint16_t motor_current_sample_handle(uint16_t current_value, uint32_t *sample) {
    static uint16_t output_current;
    uint32_t        start_time = *sample;
    uint16_t        ret        = 0;

    if (motor_max_current_output != output_current) {
        output_current = motor_max_current_output;
        start_time     = 0;
        log_i("output_current[%d]", output_current);
    }

    if (output_current == sys_paras.motor_current.motor_max_current_output) {
        if (current_value >= sys_paras.motor_current.motor_warn_current) {
            if (start_time == 0) {
                start_time = osKernelGetTickCount();
            }
        } else if (current_value < (sys_paras.motor_current.motor_warn_current - 20)) {
            start_time = 0;
        }
    } else if (output_current == sys_paras.motor_current.motor_min_current_output) {
        if (current_value <= sys_paras.motor_current.motor_normal_current) {
            if (start_time == 0) {
                start_time = osKernelGetTickCount();
            }
        } else {
            start_time = 0;
        }
    }

    if (start_time > 0) {
        if (osKernelGetTickCount() - start_time > sys_paras.motor_current.motor_current_sample_time * 1000) {
            start_time = 0;
            if (output_current == sys_paras.motor_current.motor_max_current_output) {
                ret = (uint16_t) sys_paras.motor_current.motor_min_current_output;
            } else {
                ret = (uint16_t) sys_paras.motor_current.motor_max_current_output;
            }
        }
    }
    *sample = start_time;
    return ret;
}

#ifdef __cplusplus
}
#endif

/* @} FAL_MOTOR */
/* @} Robot-FAL */
