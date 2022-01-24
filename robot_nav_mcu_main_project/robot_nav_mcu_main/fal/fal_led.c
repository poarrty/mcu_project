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
#include "fal_led.h"
#include "cmsis_os.h"
#include "sys_paras.h"
#include "bsp_led.h"
#include "sys_pubsub.h"
#include "pal_uros.h"
#include "elog.h"
#include "shell.h"
#include "chassis_interfaces/msg/ledcontrol.h"
#define NDEBUG
#include "assert.h"
/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_LED 灯控功能 - LED
 *
 * @brief FAL_LED
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define PLED_SPACE 9
#define DLED_SPACE 9
/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
static subscrption p_led_ctrl;
static subscrption d_led_ctrl;

static chassis_interfaces__msg__Ledcontrol pled_ctrl = {0};
static chassis_interfaces__msg__Ledcontrol dled_ctrl = {0};
/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
static osTimerId_t p_led_lamp_blink_timer1 = NULL;
static osTimerId_t p_led_lamp_blink_timer2 = NULL;

static osTimerId_t d_led_lamp_blink_timer = NULL;

static uint32_t pled_color_buff[PLED_SPACE];
static uint8_t  pled_hz_buff[PLED_SPACE];

static uint32_t dled_color_buff[DLED_SPACE];
static uint8_t  dled_hz_buff[DLED_SPACE];

static uint32_t period1    = 0;
static uint32_t period2    = 0;
static uint8_t  pled_flag1 = 0;
static uint8_t  pled_flag2 = 0;
/*****************************************************************
 * 函数原型声明
 ******************************************************************/
static void led_power_button_blink_cb(void *argument);
static void led_run_blink_cb(void *argument);

static void led_position_blink_cb1(void *argument);
static void led_position_blink_cb2(void *argument);
static void led_driving_blink_cb3(void *argument);
/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/
void p_led_ctrl_subscription_callback(const void *msgin);
void d_led_ctrl_subscription_callback(const void *msgin);

void Set_pled_color(uint8_t color_R[], uint8_t color_G[], uint8_t color_B[]);
void Set_dled_color(uint8_t color_R[], uint8_t color_G[], uint8_t color_B[]);
void pled_lamp_set_freq(uint8_t val[]);
void dled_lamp_set_freq(uint8_t val[]);
/*****************************************************************/
/**
 * Function:       fal_led_init
 * Description:    初始化fal_led资源
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
int fal_led_init(void) {
    subscrption_init(&p_led_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, Ledcontrol), "/led_control/position", &pled_ctrl,
                     DEFAULT, p_led_ctrl_subscription_callback);

    subscrption_init(&d_led_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, Ledcontrol), "/led_control/driving", &dled_ctrl, DEFAULT,
                     d_led_ctrl_subscription_callback);

    pled_ctrl.led_color.data     = pled_color_buff;
    pled_ctrl.led_color.size     = 0;
    pled_ctrl.led_color.capacity = sizeof(pled_color_buff);
    pled_ctrl.led_hz.data        = pled_hz_buff;
    pled_ctrl.led_hz.size        = 0;
    pled_ctrl.led_hz.capacity    = sizeof(pled_hz_buff);

    dled_ctrl.led_color.data     = dled_color_buff;
    dled_ctrl.led_color.size     = 0;
    dled_ctrl.led_color.capacity = sizeof(dled_color_buff);
    dled_ctrl.led_hz.data        = dled_hz_buff;
    dled_ctrl.led_hz.size        = 0;
    dled_ctrl.led_hz.capacity    = sizeof(dled_hz_buff);

    return 0;
}
/*****************************************************************/
void p_led_ctrl_subscription_callback(const void *msgin) {
    uint8_t pled_color_R[PLED_SPACE] = {0}, pled_color_G[PLED_SPACE] = {0}, pled_color_B[PLED_SPACE] = {0};

    const chassis_interfaces__msg__Ledcontrol *msg = (const chassis_interfaces__msg__Ledcontrol *) msgin;

    if (msg->led_color.size == PLED_SPACE && msg->led_hz.size == PLED_SPACE) {
        for (int i = 0; i < PLED_SPACE; i++) {
            pled_color_R[i] = (pled_color_buff[i] & 0x00ff0000) >> 16;
            pled_color_G[i] = (pled_color_buff[i] & 0x0000ff00) >> 8;
            pled_color_B[i] = (pled_color_buff[i] & 0x000000ff);

            log_i(
                "/led_control/position: R[0x%02x] G[0x%02x] B[0x%02x] "
                "H[0x%02x]",
                pled_color_R[i], pled_color_G[i], pled_color_B[i], pled_hz_buff[i]);
        }

        Set_pled_color(pled_color_R, pled_color_G, pled_color_B);
        pled_lamp_set_freq(pled_hz_buff);
    } else {
        log_i("Input position led's size is error!");
    }
}

void d_led_ctrl_subscription_callback(const void *msgin) {
    uint8_t dled_color_R[DLED_SPACE] = {0}, dled_color_G[DLED_SPACE] = {0}, dled_color_B[DLED_SPACE] = {0};

    const chassis_interfaces__msg__Ledcontrol *msg = (const chassis_interfaces__msg__Ledcontrol *) msgin;

    if (msg->led_color.size == DLED_SPACE && msg->led_hz.size == DLED_SPACE) {
        for (int i = 0; i < DLED_SPACE; i++) {
            dled_color_R[i] = (dled_color_buff[i] & 0x00ff0000) >> 16;
            dled_color_G[i] = (dled_color_buff[i] & 0x0000ff00) >> 8;
            dled_color_B[i] = (dled_color_buff[i] & 0x000000ff);

            log_i("/led_control/driving: R[0x%02x] G[0x%02x] B[0x%02x] H[0x%02x]", dled_color_R[i], dled_color_G[i], dled_color_B[i],
                  dled_hz_buff[i]);
        }

        Set_dled_color(dled_color_R, dled_color_G, dled_color_B);
        dled_lamp_set_freq(dled_hz_buff);
    } else {
        log_i("Input driving led's size is error!");
    }
}

/*****************************************************************/
/**
 * Function:       fal_led_deInit
 * Description:    释放fal_led资源
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
int fal_led_deInit(void) {
    return 0;
}

void task_fal_led_run(void *argument) {
    uint32_t   sub_evt;
    LED_CTRL_T led_ctrl;
    PMU_STA_T  pmu_sta;

    osTimerId_t led_power_button_blink_timer = osTimerNew(led_power_button_blink_cb, osTimerPeriodic, NULL, NULL);

    p_led_lamp_blink_timer1 = osTimerNew(led_position_blink_cb1, osTimerPeriodic, NULL, NULL);
    p_led_lamp_blink_timer2 = osTimerNew(led_position_blink_cb2, osTimerPeriodic, NULL, NULL);
    // osTimerStart(p_led_lamp_blink_timer1, 500U);
    // osTimerStart(p_led_lamp_blink_timer2, 500U);

    d_led_lamp_blink_timer = osTimerNew(led_driving_blink_cb3, osTimerPeriodic, NULL, NULL);
    // osTimerStart(d_led_lamp_blink_timer, 500U);

    osTimerId_t led_run_timer;
    led_run_timer = osTimerNew(led_run_blink_cb, osTimerPeriodic, NULL, NULL);
    osTimerStart(led_run_timer, 500U);

    fal_led_init();

    for (;;) {
        ///< 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_led, SYS_EVT_LED_CONTROL | SYS_EVT_PMU_STA, osFlagsWaitAny, 100);

        if (sub_evt & osFlagsError) {
            continue;
        }

        ///< 读取主题数据

        ///< 灯光颜色控制主题
        if (sub_evt & SYS_EVT_LED_CONTROL) {
            xQueuePeek(topic_led, &led_ctrl, 100);

            bsp_lamp_color(led_ctrl.lamp, led_ctrl.color);
        }

        ///< 开关机状态主题
        if (sub_evt & SYS_EVT_PMU_STA) {
            xQueuePeek(topic_pmu_sta, &pmu_sta, 100);

            if (pmu_sta.sta == PMU_STA_STARTING || pmu_sta.sta == PMU_STA_SHUTDOWN) {
                if (pmu_sta.sta == PMU_STA_STARTING) {
                    bsp_led_on(LED_POWER_BUTTON);
                } else {
                    bsp_led_off(LED_POWER_BUTTON);
                }

                osTimerStart(led_power_button_blink_timer, 500U);
            } else if (pmu_sta.sta == PMU_STA_STANDBY || pmu_sta.sta == PMU_STA_RUNNING) {
                osTimerStop(led_power_button_blink_timer);

                if (pmu_sta.sta == PMU_STA_STANDBY) {
                    bsp_led_off(LED_POWER_BUTTON);
                    bsp_led_off(LED_DRV);

                    osTimerStop(p_led_lamp_blink_timer1);
                    osTimerStop(p_led_lamp_blink_timer2);
                    bsp_lamp_color(LAMP_Left, COLOR_OFF);
                    bsp_lamp_color(LAMP_Right, COLOR_OFF);
                } else {
                    bsp_led_on(LED_POWER_BUTTON);
                }
            }
        }
    }
}

static void led_power_button_blink_cb(void *argument) {
    bsp_led_toggle(LED_POWER_BUTTON);
}

static void led_position_blink_cb1(void *argument) {
    if (pled_flag1) {
        ((LED_COLOR_E) sys_paras.left_color_r == 0) ? bsp_led_off(LED_LEFT_R) : bsp_led_on(LED_LEFT_R);
        ((LED_COLOR_E) sys_paras.left_color_g == 0) ? bsp_led_off(LED_LEFT_G) : bsp_led_on(LED_LEFT_G);
        ((LED_COLOR_E) sys_paras.left_color_b == 0) ? bsp_led_off(LED_LEFT_B) : bsp_led_on(LED_LEFT_B);
    } else {
        bsp_led_off(LED_LEFT_R);
        bsp_led_off(LED_LEFT_G);
        bsp_led_off(LED_LEFT_B);
    }

    pled_flag1 = !pled_flag1;
}

static void led_position_blink_cb2(void *argument) {
    if (pled_flag2) {
        ((LED_COLOR_E) sys_paras.right_color_r == 0) ? bsp_led_off(LED_RIGHT_R) : bsp_led_on(LED_RIGHT_R);
        ((LED_COLOR_E) sys_paras.right_color_g == 0) ? bsp_led_off(LED_RIGHT_G) : bsp_led_on(LED_RIGHT_G);
        ((LED_COLOR_E) sys_paras.right_color_b == 0) ? bsp_led_off(LED_RIGHT_B) : bsp_led_on(LED_RIGHT_B);

    } else {
        bsp_led_off(LED_RIGHT_R);
        bsp_led_off(LED_RIGHT_G);
        bsp_led_off(LED_RIGHT_B);
    }

    pled_flag2 = !pled_flag2;
}

void pled_lamp_set_freq(uint8_t val[]) {
    static uint8_t Hz     = 0XFF;
    COLOR_RGB      LEFT_R = 0, LEFT_G = 0, LEFT_B = 0;
    COLOR_RGB      RIGHT_R = 0, RIGHT_G = 0, RIGHT_B = 0;

    for (int i = 0; i < PLED_SPACE; i++) {
        switch (i) {
            case LAMP_Left:
                if (p_led_lamp_blink_timer1 == NULL) {
                    bsp_led_off(LED_LEFT_R);
                    bsp_led_off(LED_LEFT_G);
                    bsp_led_off(LED_LEFT_B);

                    return;
                }

                if (Hz == val[i] && LEFT_R == sys_paras.left_color_r && LEFT_G == sys_paras.left_color_g && LEFT_B == sys_paras.left_color_b) {
                    return;
                } else {
                    LEFT_R = (LED_COLOR_E) sys_paras.left_color_r;
                    LEFT_G = (LED_COLOR_E) sys_paras.left_color_g;
                    LEFT_B = (LED_COLOR_E) sys_paras.left_color_b;

                    Hz = val[i];
                }

                if (Hz == 0 || Hz > 30) {
                    osTimerStop(p_led_lamp_blink_timer1);
                    bsp_led_off(LED_LEFT_R);
                    bsp_led_off(LED_LEFT_G);
                    bsp_led_off(LED_LEFT_B);
                } else if (Hz == 30) {
                    osTimerStop(p_led_lamp_blink_timer1);
                    ((LED_COLOR_E) sys_paras.left_color_r == 0) ? bsp_led_off(LED_LEFT_R) : bsp_led_on(LED_LEFT_R);
                    ((LED_COLOR_E) sys_paras.left_color_g == 0) ? bsp_led_off(LED_LEFT_G) : bsp_led_on(LED_LEFT_G);
                    ((LED_COLOR_E) sys_paras.left_color_b == 0) ? bsp_led_off(LED_LEFT_B) : bsp_led_on(LED_LEFT_B);
                } else {
                    period1 = 1000.0 / Hz / 2.0;

                    pled_flag1 = 0;
                    pled_flag2 = 0;

                    osTimerStart(p_led_lamp_blink_timer1, period1);
                }

                break;

            case LAMP_Right:
                if (p_led_lamp_blink_timer2 == NULL) {
                    bsp_led_off(LED_RIGHT_R);
                    bsp_led_off(LED_RIGHT_G);
                    bsp_led_off(LED_RIGHT_B);

                    return;
                }

                if (Hz == val[i] && RIGHT_R == sys_paras.right_color_r && RIGHT_G == sys_paras.right_color_g &&
                    RIGHT_B == sys_paras.right_color_b) {
                    return;
                } else {
                    RIGHT_R = (LED_COLOR_E) sys_paras.right_color_r;
                    RIGHT_G = (LED_COLOR_E) sys_paras.right_color_g;
                    RIGHT_B = (LED_COLOR_E) sys_paras.right_color_b;

                    Hz = val[i];
                }

                if (Hz == 0 || Hz > 30) {
                    osTimerStop(p_led_lamp_blink_timer2);
                    bsp_led_off(LED_RIGHT_R);
                    bsp_led_off(LED_RIGHT_G);
                    bsp_led_off(LED_RIGHT_B);
                } else if (Hz == 30) {
                    osTimerStop(p_led_lamp_blink_timer2);
                    ((LED_COLOR_E) sys_paras.right_color_r == 0) ? bsp_led_off(LED_RIGHT_R) : bsp_led_on(LED_RIGHT_R);
                    ((LED_COLOR_E) sys_paras.right_color_g == 0) ? bsp_led_off(LED_RIGHT_G) : bsp_led_on(LED_RIGHT_G);
                    ((LED_COLOR_E) sys_paras.right_color_b == 0) ? bsp_led_off(LED_RIGHT_B) : bsp_led_on(LED_RIGHT_B);
                } else {
                    period2 = 1000.0 / Hz / 2.0;

                    pled_flag1 = 0;
                    pled_flag2 = 0;

                    osTimerStart(p_led_lamp_blink_timer2, period2);
                }

                break;

            default:
                break;
        }
    }
}

void test_pled_freq(uint8_t val) {
    uint8_t led_freq[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch (val) {
        case 1:
            led_freq[1] = 1;
            break;

        case 2:
            led_freq[2] = 1;
            break;

        case 3:
            led_freq[1] = 1;
            led_freq[2] = 1;
            break;

        case 4:
            led_freq[1] = 1;
            led_freq[2] = 30;
            break;

        case 5:
            led_freq[1] = 30;
            led_freq[2] = 1;
            break;

        case 6:
            led_freq[1] = 0;
            led_freq[2] = 0;
            break;

        default:
            break;
    }

    pled_lamp_set_freq(led_freq);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_pled_freq, test_pled_freq, test_pled_freq);
/*************************************************************************************************************/
static void led_driving_blink_cb3(void *argument) {
    static uint8_t flag = 0;

    if (flag) {
        bsp_led_on(LED_DRV);
    } else {
        bsp_led_off(LED_DRV);
    }

    flag = !flag;
}

void dled_lamp_set_freq(uint8_t val[]) {
    static uint8_t Hz = 0XFF;

    for (int i = 0; i < DLED_SPACE; i++) {
        switch (i) {
            case LAMP_Up:
                if (d_led_lamp_blink_timer == NULL) {
                    bsp_led_off(LED_DRV);

                    return;
                }

                Hz = val[i];

                if (Hz == 0 || Hz > 30) {
                    osTimerStop(d_led_lamp_blink_timer);
                    bsp_led_off(LED_DRV);
                } else if (Hz >= 20 && Hz <= 30) {
                    osTimerStop(d_led_lamp_blink_timer);
                    bsp_led_on(LED_DRV);
                } else {
                    uint32_t period = 1000.0 / Hz / 2.0;

                    osTimerStart(d_led_lamp_blink_timer, period);
                }

                break;

            default:
                break;
        }
    }
}

void test_dled_freq(uint8_t val) {
    uint8_t led_freq[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch (val) {
        case 0:
            led_freq[3] = 0;
            break;

        case 1:
            led_freq[3] = 1;
            break;

        case 2:
            led_freq[3] = 15;
            break;

        case 3:
            led_freq[3] = 30;
            break;

        default:
            break;
    }

    dled_lamp_set_freq(led_freq);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_dled_freq, test_dled_freq, test_dled_freq);
/*************************************************************************************************************/
static void led_run_blink_cb(void *argument) {
    bsp_led_toggle(LED_RUN);
}
/*************************************************************************************************************/
void Set_pled_color(uint8_t color_R[], uint8_t color_G[], uint8_t color_B[]) {
    for (int i = 0; i < PLED_SPACE; i++) {
        switch (i) {
            case LAMP_Left:
                sys_paras.left_color_r = ((color_R[i] == 0) ? 0 : 1);
                sys_paras.left_color_g = ((color_G[i] == 0) ? 0 : 1);
                sys_paras.left_color_b = ((color_B[i] == 0) ? 0 : 1);

                ((LED_COLOR_E) sys_paras.left_color_r == 0) ? bsp_led_off(LED_LEFT_R) : bsp_led_on(LED_LEFT_R);
                ((LED_COLOR_E) sys_paras.left_color_g == 0) ? bsp_led_off(LED_LEFT_G) : bsp_led_on(LED_LEFT_G);
                ((LED_COLOR_E) sys_paras.left_color_b == 0) ? bsp_led_off(LED_LEFT_B) : bsp_led_on(LED_LEFT_B);

                break;

            case LAMP_Right:
                sys_paras.right_color_r = ((color_R[i] == 0) ? 0 : 1);
                sys_paras.right_color_g = ((color_G[i] == 0) ? 0 : 1);
                sys_paras.right_color_b = ((color_B[i] == 0) ? 0 : 1);

                ((LED_COLOR_E) sys_paras.right_color_r == 0) ? bsp_led_off(LED_RIGHT_R) : bsp_led_on(LED_RIGHT_R);
                ((LED_COLOR_E) sys_paras.right_color_g == 0) ? bsp_led_off(LED_RIGHT_G) : bsp_led_on(LED_RIGHT_G);
                ((LED_COLOR_E) sys_paras.right_color_b == 0) ? bsp_led_off(LED_RIGHT_B) : bsp_led_on(LED_RIGHT_B);

                break;

            default:
                break;
        }
    }
}

void test_pled_rgb(uint8_t val) {
    uint8_t color_R[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t color_G[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t color_B[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch (val) {
        case 1:
            color_R[1] = 1;
            color_G[1] = 1;
            color_B[1] = 1;

            break;

        case 2:
            color_R[2] = 1;
            color_G[2] = 1;
            color_B[2] = 1;

            break;

        case 3:
            color_R[1] = 1;
            color_G[1] = 1;
            color_B[1] = 1;

            color_R[2] = 1;
            color_G[2] = 1;
            color_B[2] = 1;

            break;

        case 4:
            color_R[1] = 1;
            color_G[1] = 0;
            color_B[1] = 1;

            color_R[2] = 1;
            color_G[2] = 1;
            color_B[2] = 0;

            break;

        case 5:
            color_R[1] = 0;
            color_G[1] = 0;
            color_B[1] = 1;

            color_R[2] = 0;
            color_G[2] = 1;
            color_B[2] = 0;

            break;

        case 6:
            color_R[1] = 0;
            color_G[1] = 0;
            color_B[1] = 0;

            color_R[2] = 0;
            color_G[2] = 0;
            color_B[2] = 0;

            break;

        default:
            break;
    }

    Set_pled_color(color_R, color_G, color_B);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_pled_rgb, test_pled_rgb, test_pled_rgb);
/***************************************************************************************/
void Set_dled_color(uint8_t color_R[], uint8_t color_G[], uint8_t color_B[]) {
    for (int i = 0; i < DLED_SPACE; i++) {
        switch (i) {
            case LAMP_Up:
                if (color_R[i] == 0 && color_G[i] == 0 && color_B[i] == 0) {
                    bsp_led_off(LED_DRV);
                } else if (color_R[i] != 0 && color_G[i] != 0 && color_B[i] != 0) {
                    bsp_led_on(LED_DRV);
                } else {
                    log_i("Driving light's color control is error!");
                }

                break;

            default:
                break;
        }
    }
}

void test_dled_rgb(uint8_t val) {
    uint8_t color_R[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t color_G[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t color_B[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch (val) {
        case 0:
            color_R[3] = 0;
            color_G[3] = 0;
            color_B[3] = 0;

            break;

        case 1:
            color_R[3] = 1;
            color_G[3] = 1;
            color_B[3] = 1;

            break;

        default:
            break;
    }

    Set_dled_color(color_R, color_G, color_B);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_dled_rgb, test_dled_rgb, test_dled_rgb);
/**********************************************************************************************/
#ifdef __cpluspluste
}
#endif

/* @} FAL_LED */
/* @} Robot_FAL */
