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
#include "fal_display.h"
#include "cmsis_os.h"
#include "sys_paras.h"
#include "bsp_led.h"
#include "spi.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "elog.h"
#include "sys_exc.h"
#include "bsp_battery.h"
#include "sys_pubsub.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_DISPLAY 显示功能（数码管） - DISPLAY
 *
 * @brief
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define DIGITAL_NUM 4

/* 数码管字符显示 */
#define DISPLAY_NUM0 0xC0  // 0x3F
#define DISPLAY_NUM1 0xF9  // 0x06
#define DISPLAY_NUM2 0xA4  // 0x5B
#define DISPLAY_NUM3 0xB0  // 0x4F
#define DISPLAY_NUM4 0x99  // 0x66
#define DISPLAY_NUM5 0x92  // 0x6D
#define DISPLAY_NUM6 0x82  // 0x7D
#define DISPLAY_NUM7 0xF8  // 0x07
#define DISPLAY_NUM8 0x80  // 0x7F
#define DISPLAY_NUM9 0x90  // 0x6F
#define DISPLAY_NUMA 0x88  // 0x77
#define DISPLAY_NUMB 0x83  // 0x7C
#define DISPLAY_NUMC 0xC6  // 0x39
#define DISPLAY_NUMD 0xA1  // 0x5E
#define DISPLAY_NUME 0x86  // 0x79
#define DISPLAY_NUMF 0x84  // 0x71
#define DISPLAY_NUMH 0xFF  // 0x76
#define DISPLAY_NUMP 0xBF  // 0xF3

/* 电量进度条显示 */
#define BATTERY_PERCENT0 0xFF /* 0% */
#define BATTERY_PERCENT1 0x7F /* 0-12.5% */
#define BATTERY_PERCENT2 0x3F /* 12.5-25% */
#define BATTERY_PERCENT3 0x1F /* 25-37.5% */
#define BATTERY_PERCENT4 0x0F /* 37.5-50% */
#define BATTERY_PERCENT5 0x07 /* 50-62.5% */
#define BATTERY_PERCENT6 0x03 /* 62.5-75% */
#define BATTERY_PERCENT7 0x01 /* 75-87.5% */
#define BATTERY_PERCENT8 0x00 /* 87.5-100% */

#define DIS_CHOICE_0 (0xFF & (~(0x01 << 0)))
#define DIS_CHOICE_1 (0xFF & (~(0x01 << 1)))
#define DIS_CHOICE_2 (0xFF & (~(0x01 << 2)))
#define DIS_CHOICE_3 (0xFF & (~(0x01 << 3)))

#define DIS_WITH_OUTLOOK (0xFF & (~(0x01 << 4)))
#define DIS_WITH_PERCENT (0xFF & (~(0x01 << 5)))
#define DIS_WITH_VOLTAGE (0xFF & (~(0x01 << 6)))

#define DISPLAY_BUFF_UPDATE_PERIOD (500)  ///< 显存更新周期/ms

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
DISPLAY_CMD_T display_ctrl = {0};

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
// static osTimerId_t timer_display_flash;
static uint8_t display_update_sta = 0;

static const unsigned char DIGITAL_NUM_CODE[16] = {DISPLAY_NUM0, DISPLAY_NUM1, DISPLAY_NUM2, DISPLAY_NUM3, DISPLAY_NUM4, DISPLAY_NUM5,
                                                   DISPLAY_NUM6, DISPLAY_NUM7, DISPLAY_NUM8, DISPLAY_NUM9, DISPLAY_NUMA, DISPLAY_NUMB,
                                                   DISPLAY_NUMC, DISPLAY_NUMD, DISPLAY_NUME, DISPLAY_NUMF};

static const unsigned char DIGITAL_BAR_CODE[9] = {BATTERY_PERCENT0, BATTERY_PERCENT1, BATTERY_PERCENT2, BATTERY_PERCENT3, BATTERY_PERCENT4,
                                                  BATTERY_PERCENT5, BATTERY_PERCENT6, BATTERY_PERCENT7, BATTERY_PERCENT8};

static osMutexId_t mutex_battery_display;
static osMutexId_t mutex_error_display;

const osMutexAttr_t mutex_battery_display_attr = {
    "mutex_battery_display",                // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

const osMutexAttr_t mutex_error_display_attr = {
    "mutex_error_display",                  // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

///< 数码管显存
static uint8_t display_choice_buff[DIGITAL_NUM] = {
    DIS_CHOICE_0 & DIS_WITH_OUTLOOK & DIS_WITH_PERCENT,
    DIS_CHOICE_1 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
    DIS_CHOICE_2 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
    DIS_CHOICE_3 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
};
static uint8_t display_show_buff[DIGITAL_NUM] = {DISPLAY_NUM8, DISPLAY_NUM8, DISPLAY_NUM8, BATTERY_PERCENT8};

///< 电量显示缓存
static uint8_t battery_choice_buff[DIGITAL_NUM] = {
    DIS_CHOICE_0 & DIS_WITH_OUTLOOK & DIS_WITH_PERCENT,
    DIS_CHOICE_1 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
    DIS_CHOICE_2 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
    DIS_CHOICE_3 &DIS_WITH_OUTLOOK &DIS_WITH_PERCENT,
};
static uint8_t battery_show_buff[DIGITAL_NUM] = {DISPLAY_NUM8, DISPLAY_NUM8, DISPLAY_NUM8, BATTERY_PERCENT8};

///< 故障显示缓存
static uint8_t error_choice_buff[DIGITAL_NUM] = {
    DIS_CHOICE_0 & DIS_WITH_OUTLOOK,
    DIS_CHOICE_1 &DIS_WITH_OUTLOOK,
    DIS_CHOICE_2 &DIS_WITH_OUTLOOK,
    DIS_CHOICE_3 &DIS_WITH_OUTLOOK,
};
static uint8_t error_show_buff[DIGITAL_NUM] = {DISPLAY_NUM8, DISPLAY_NUM8, DISPLAY_NUM8, BATTERY_PERCENT8};

///< SOC显示缓存
static uint8_t soc_choice_buff[DIGITAL_NUM] = {
    DIS_CHOICE_0 & DIS_WITH_OUTLOOK,
    DIS_CHOICE_1 &DIS_WITH_OUTLOOK,
    DIS_CHOICE_2 &DIS_WITH_OUTLOOK,
    DIS_CHOICE_3 &DIS_WITH_OUTLOOK,
};
// static uint8_t soc_show_buff[DIGITAL_NUM] = {DISPLAY_NUM8, DISPLAY_NUM8,
// DISPLAY_NUM8, BATTERY_PERCENT8};

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
static void display_flash_cb(void *argument);  ///< 视觉暂存刷新定时处理
static void display_buff_update(void);         ///< 显示内容刷新处理

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_display_init
 * Description:    初始化 fal_display
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
int fal_display_init(void) {
    mutex_battery_display = osMutexNew(&mutex_battery_display_attr);
    mutex_error_display   = osMutexNew(&mutex_error_display_attr);

    if (mutex_battery_display == NULL || mutex_error_display == NULL) {
        log_e("Display mutex create failed.");
    } else {
        // log_i("Display mutex create ok.");
    }

    ///< 未知的问题，在定时器中刷新会有异常闪烁，改到任务中进行刷新。（将定位器优先级提高和将日志任务优先级降低会改善很多，但仍会有一点闪烁）
    // timer_display_flash = osTimerNew(display_flash_cb, osTimerPeriodic, NULL,
    // NULL); osTimerStart(timer_display_flash, 5U);          ///<
    // 视觉暂存更新周期, 有4组数码管，4*5ms = 20ms --> 50HZ

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_display_deInit
 * Description:    释放 fal_display 资源
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
int fal_display_deInit(void) {
    return 0;
}

void task_fal_display_run(void *argument) {
    uint32_t       sub_evt;
    PMU_STA_T      pmu_sta;
    DISPLAY_CTRL_T ctrl;
    // BATT_STA_T batt_sta;

    uint8_t display_update_sta_save = 0;

    fal_display_init();

    for (;;) {
        ///< 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_display, SYS_EVT_PMU_STA | SYS_EVT_DISPLAY_CTRL | SYS_EVT_BATT_STA, osFlagsWaitAny, 5);

        if ((sub_evt & osFlagsError) == 0) {
            ///< 读取主题数据
            ///< 开关机状态主题
            if (sub_evt & SYS_EVT_PMU_STA) {
                xQueuePeek(topic_pmu_sta, &pmu_sta, 100);

                ///< 不再显示开关机计时
                /*
                if(pmu_sta.sta == PMU_STA_STARTING || pmu_sta.sta ==
                PMU_STA_SHUTDOWN)
                {
                    if(pmu_sta.sta == PMU_STA_STARTING)
                        display_update_sta = 3;
                    else
                        display_update_sta = 4;
                }
                else if(pmu_sta.sta == PMU_STA_STANDBY || pmu_sta.sta ==
                PMU_STA_RUNNING)
                {
                    if(pmu_sta.sta == PMU_STA_STANDBY)
                       ;
                    else
                       ;

                    display_update_sta = 0;
                }
                */
            }

            ///< 显示控制主题
            if (sub_evt & SYS_EVT_DISPLAY_CTRL) {
                xQueuePeek(topic_display_ctrl, &ctrl, 100);

                display_ctrl = ctrl.cmd;

                if (ctrl.cmd.display_mode == 1) {
                    if (display_update_sta != 5) {
                        display_update_sta_save = display_update_sta;
                        display_update_sta      = 5;
                    }
                } else {
                    if (display_update_sta == 5) {
                        display_update_sta = display_update_sta_save;
                    }
                }
            }

            /*
            if(sub_evt & SYS_EVT_BATT_STA)
            {
                xQueuePeek(topic_batt_sta, &batt_sta, 100);

                if(batt_sta.sta == BATT_LOW_DETECT && fal_pmu_get_sta() ==
            PMU_STA_STANDBY)
                {
                    if(display_update_sta != 6)
                    {
                        display_update_sta_save = display_update_sta;
                        display_update_sta = 6;
                    }
                }
                else
                {
                    if(display_update_sta == 6)
                    {
                        display_update_sta = display_update_sta_save;
                    }
                }
            }
            */
        }

        display_flash_cb(NULL);
    }
}

static void display_flash_cb(void *argument) {
    static uint32_t ts    = 0;
    static uint8_t  index = 0;
    static uint8_t  buff[2];
    uint8_t         lock_delay;

    ///< 显示内容长周期刷新
    if (osKernelGetTickCount() >= ts + DISPLAY_BUFF_UPDATE_PERIOD) {
        display_buff_update();

        ts = osKernelGetTickCount() + DISPLAY_BUFF_UPDATE_PERIOD;
    }

    ///< 显存推送到数码管
    ///< 视觉暂存短周期刷新
    buff[0] = display_choice_buff[index];
    buff[1] = display_show_buff[index];

    ///< 给 74HC595 发送数据
    HAL_SPI_Transmit(&hspi1, buff, 2, 100);

    ///< 给 74HC595 发送数据锁存信号，数据锁存并输出到引脚
    HAL_GPIO_WritePin(MX_DISPLAY_LOCK_GPIO_Port, MX_DISPLAY_LOCK_Pin, GPIO_PIN_RESET);
    ///< 锁存延时，约 1 us
    lock_delay = 30;
    while (lock_delay--)
        ;
    HAL_GPIO_WritePin(MX_DISPLAY_LOCK_GPIO_Port, MX_DISPLAY_LOCK_Pin, GPIO_PIN_SET);

    index++;

    if (index >= DIGITAL_NUM) {
        index = 0;
    }
}

/// 显存更新
static void display_buff_update(void) {
    static uint8_t sys_exc_poll = 1;
    uint8_t        battery_percent;
    uint8_t        battery_charge;

    battery_percent = bsp_battery_volatage_percent_get();
    battery_charge  = bsp_battery_charge_sta_get();

    //将电量信息更新到电量相关显存
    display_set_battery_percent(battery_charge, battery_percent);

    if (osMutexAcquire(mutex_battery_display, 0) != osOK)  ///< 定时器任务不阻塞等待
    {
        return;
    }

    display_show_buff[3] = battery_show_buff[3];  ///< 电量条状态每次从电量相关缓存获取
    osMutexRelease(mutex_battery_display);

    //根据当前显示状态选择显存
    switch (display_update_sta) {
        //显示电量百分比显存
        case 0:

            if (osMutexAcquire(mutex_battery_display, 0) != osOK) {
                return;
            }

            memcpy(display_choice_buff, battery_choice_buff, DIGITAL_NUM);
            memcpy(display_show_buff, battery_show_buff, DIGITAL_NUM);
            osMutexRelease(mutex_battery_display);

            //只在开机完成状态显示错误码
            //检查后续还有没有错误码
            if (fal_pmu_get_sta() == PMU_STA_RUNNING) {
                uint8_t i;

                for (i = 1; i <= EXC_NUM; i++) {
                    if (sys_exc_get(i) == true) {
                        break;
                    }
                }

                //没有错误码则下一个周期不进行错误码查询
                if (i > EXC_NUM) {
                    sys_exc_poll       = 1;
                    display_update_sta = 0;
                } else {
                    sys_exc_poll       = 1;
                    display_update_sta = 1;
                }
            }

            break;

        //显示故障码显存
        case 1:

            for (; sys_exc_poll <= EXC_NUM; sys_exc_poll++) {
                if (sys_exc_get(sys_exc_poll) == true) {
                    //设置要显示的错误码
                    display_set_error_code(sys_exc_poll);
                    break;
                }
            }

            if (sys_exc_poll > EXC_NUM) {
                sys_exc_poll       = 1;
                display_update_sta = 0;
            } else {
                if (osMutexAcquire(mutex_error_display, 0) != osOK) {
                    return;
                }

                memcpy(display_choice_buff, error_choice_buff, DIGITAL_NUM);
                memcpy(display_show_buff, error_show_buff,
                       DIGITAL_NUM - 1);  ///< 不改变电量条
                osMutexRelease(mutex_error_display);

                //已显示的错误码跳过
                sys_exc_poll++;

                //检查后续还有没有错误码
                uint8_t i;

                for (i = sys_exc_poll; i <= EXC_NUM; i++) {
                    if (sys_exc_get(i) == true) {
                        break;
                    }
                }

                //没有错误码则下一个周期不再进行错误码查询
                if (i > EXC_NUM) {
                    sys_exc_poll       = 1;
                    display_update_sta = 0;
                }
            }

            break;

        //显示开机计时
        case 3:

        {
            uint32_t time = fal_pmu_sys_up_time() / 1000;

            uint8_t hundred = time / 100 % 10;
            uint8_t ten     = time / 10 % 10;
            uint8_t single  = time % 10;

            memcpy(display_choice_buff, error_choice_buff,
                   DIGITAL_NUM);  ///< 跟错误码一样不要百分号
            display_show_buff[0] = DIGITAL_NUM_CODE[hundred];
            display_show_buff[1] = DIGITAL_NUM_CODE[ten];
            display_show_buff[2] = DIGITAL_NUM_CODE[single];
        }

        break;

        //显示关机计时
        case 4: {
            uint32_t time = fal_pmu_sys_down_time() / 1000;

            uint8_t hundred = time / 100 % 10;
            uint8_t ten     = time / 10 % 10;
            uint8_t single  = time % 10;

            memcpy(display_choice_buff, error_choice_buff,
                   DIGITAL_NUM);  ///< 跟错误码一样不要百分号
            display_show_buff[0] = DIGITAL_NUM_CODE[hundred];
            display_show_buff[1] = DIGITAL_NUM_CODE[ten];
            display_show_buff[2] = DIGITAL_NUM_CODE[single];
        }

        break;

        //显示 SOC 指定内容
        case 5: {
            memcpy(display_choice_buff, soc_choice_buff, DIGITAL_NUM);

            if (display_ctrl.hundred == 0XFF) {
                display_show_buff[0] = 0XFF;
            } else if (display_ctrl.hundred <= 15) {
                display_show_buff[0] = DIGITAL_NUM_CODE[display_ctrl.hundred];
            }

            if (display_ctrl.ten == 0XFF) {
                display_show_buff[1] = 0XFF;
            } else if (display_ctrl.ten <= 15) {
                display_show_buff[1] = DIGITAL_NUM_CODE[display_ctrl.ten];
            }

            if (display_ctrl.single == 0XFF) {
                display_show_buff[2] = 0XFF;
            } else if (display_ctrl.single <= 15) {
                display_show_buff[2] = DIGITAL_NUM_CODE[display_ctrl.single];
            }
        }

        break;

        default:
            break;
    }
}

uint8_t soc_display_cmd(uint8_t mode, uint8_t h, uint8_t t, uint8_t s) {
    DISPLAY_CTRL_T ctrl;

    if (mode == 0 || mode == 1) {
        ctrl.cmd.display_mode = mode;
        ctrl.cmd.hundred      = h;
        ctrl.cmd.ten          = t;
        ctrl.cmd.single       = s;

        pub_topic(SYS_EVT_DISPLAY_CTRL, &ctrl);
    }

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), display_cmd, soc_display_cmd, soc_display_cmd);

uint8_t display_set_battery_percent(uint8_t charge_sta, uint8_t val) {
    static uint8_t bar_inc = 0;
    uint8_t        hundred, ten, single;
    uint8_t        bat_bar_num = 0;

    if (val > 100) {
        val = 100;
    }

    hundred = val / 100;
    ten     = val / 10 % 10;
    single  = val % 10;

    if (val == 0) {
        bat_bar_num = 0;
    } else if (val <= 12) {
        bat_bar_num = 1;
    } else if (val <= 25) {
        bat_bar_num = 2;
    } else if (val <= 37) {
        bat_bar_num = 3;
    } else if (val <= 50) {
        bat_bar_num = 4;
    } else if (val <= 62) {
        bat_bar_num = 5;
    } else if (val <= 75) {
        bat_bar_num = 6;
    } else if (val <= 87) {
        bat_bar_num = 7;
    } else if (val <= 100) {
        bat_bar_num = 8;
    }

    if (osMutexAcquire(mutex_battery_display, 0) != osOK) {
        return 1;
    }

    if (hundred == 0) {
        battery_show_buff[0] = 0XFF;
    } else {
        battery_show_buff[0] = DIGITAL_NUM_CODE[hundred];
    }

    battery_show_buff[1] = DIGITAL_NUM_CODE[ten];
    battery_show_buff[2] = DIGITAL_NUM_CODE[single];

    //充电中，进度条递增
    if (charge_sta) {
        bat_bar_num += bar_inc;

        bar_inc++;

        if (bat_bar_num >= 8) {
            bar_inc = 0;
        }

        battery_show_buff[3] = DIGITAL_BAR_CODE[bat_bar_num];

    }
    //非充电，进度条正常显示
    else {
        bar_inc = 0;

        battery_show_buff[3] = DIGITAL_BAR_CODE[bat_bar_num];
    }

    osMutexRelease(mutex_battery_display);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), disPercent, display_set_battery_percent,
                 display_set_battery_percent);

uint8_t display_set_error_code(uint8_t val) {
    uint8_t ten, single;

    if (val > 99) {
        val = 99;
    }

    ten    = val / 10 % 10;
    single = val % 10;

    if (osMutexAcquire(mutex_error_display, 0) != osOK) {
        return 1;
    }

    error_show_buff[0] = DIGITAL_NUM_CODE[0x0E];
    error_show_buff[1] = DIGITAL_NUM_CODE[ten];
    error_show_buff[2] = DIGITAL_NUM_CODE[single];

    osMutexRelease(mutex_error_display);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), disError, display_set_error_code, display_set_error_code);

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_407 */
