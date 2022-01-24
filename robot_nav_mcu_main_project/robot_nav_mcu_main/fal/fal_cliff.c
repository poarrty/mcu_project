/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      fal.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-05-07
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_cliff.h"
#include "cmsis_os.h"
#include "lwrb.h"
#include "usart.h"
#include "shell.h"
#include "sys_pubsub.h"
#include "sys_paras.h"
#include "sys_exc.h"
#include "math.h"
#include "pal_uros.h"
#include "sensor_msgs/msg/range.h"

#define LOG_TAG "fal_cliff"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_CLIFF - CLIFF
 *
 * @brief 断崖检测功能模块\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define CLIFF_RECV_BUFF_SIZE 256

#define LASER_AMP_OVERLOAD 65535

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
lwrb_t cliff_rbuff;

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
static uint8_t     cliff_recv_buff[CLIFF_RECV_BUFF_SIZE];
CLIFF_FRMAE_DATA_T cliff_data = {0};

static bool is_dist_old_save = false;
static bool is_bump_detected = false;

static osTimerId_t cliff_judge_timer = NULL;

static publisher               g_uros_cliff;
static sensor_msgs__msg__Range uros_cliff;

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void           cliff_data_handle(uint8_t data);
static uint8_t cliff_frame_checksum(uint8_t *data, uint16_t len);

extern uint32_t fal_pmu_sys_up_time(void);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/
#if 0
static void cliff_judge_cb(void *argument);
static void fal_cliff_data_public(void);
#endif
/*****************************************************************/
/**
 * Function:       fal_cliff_init
 * Description:    初始化 fal_cliff
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
int fal_cliff_init(void) {
    publisher_init(&g_uros_cliff, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range), "/cliff", &uros_cliff, BEST, OVERWRITE,
                   sizeof(sensor_msgs__msg__Range));

    /*添加模块处理函数*/
    lwrb_init(&cliff_rbuff, cliff_recv_buff, sizeof(cliff_recv_buff));

    // cliff_judge_timer = osTimerNew(cliff_judge_cb, osTimerOnce, NULL, NULL);

    ///< 启动串口接收， 模拟串口，默认开机启动
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);  ///< 打开屏蔽则关闭此功能

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_cliff_deInit
 * Description:    释放 fal_cliff 资源
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
int fal_cliff_deInit(void) {
    return 0;
}

void task_fal_cliff_run(void *argument) {
    uint32_t      sub_evt;
    BUMP_DETECT_T bump_detect = {0};
    PMU_STA_T     pmu_sta     = {0};

    uint8_t data;

    uint32_t print_ts = 0;

    fal_cliff_init();

    for (;;) {
        ///< 检测订阅的主题
        sub_evt = osEventFlagsWait(sys_suber_cliff, SYS_EVT_BUMP_DETECT | SYS_EVT_PMU_STA, osFlagsWaitAny, 5);

        if ((sub_evt & osFlagsError) == 0) {
            ///< 读取主题数据
            if (sub_evt & SYS_EVT_BUMP_DETECT) {
                xQueuePeek(topic_bump_detect, &bump_detect, 100);

                if (bump_detect.is_bump_detected) {
                    log_i("cliff get bump detect set");

                    is_bump_detected = true;
                } else {
                    log_i("cliff get bump detect clear");
                    is_bump_detected = false;
                }
            }

            if (sub_evt & SYS_EVT_PMU_STA) {
                xQueuePeek(topic_pmu_sta, &pmu_sta, 100);

                if (pmu_sta.sta == PMU_STA_STARTING) {
                    is_dist_old_save = false;
                }
            }
        }

        while (1) {
            // fal_cliff_data_public();

            // consume data
            if (lwrb_read(&cliff_rbuff, &data, 1)) {
                cliff_data_handle(data);
            }

            else {
                break;
            }
        }

        if (osKernelGetTickCount() >= print_ts + 1000) {
            log_i("cliff data: dist[%d]cm, amp[%d], temp[%d] °C", cliff_data.dist, cliff_data.amp, cliff_data.temp / 8 - 256);

            print_ts = osKernelGetTickCount();
        }
    }
}
#if 0
static void cliff_judge_cb(void *argument)
{
	if (is_bump_detected == false)
	{
		log_i("cliff detect cliff set in timer");

		sys_exc_set(EXC66_CLIFF);
	}

}
#endif
static void cliff_data_decode_callback(CLIFF_FRMAE_DATA_P data) {
    static uint32_t cliff_judge_ts = 0;
    static uint16_t dist_old;

    ///< 激光过曝，数据无效
    if (data->amp == 65535) {
        return;
    }

    cliff_data = *data;

    if (osKernelGetTickCount() >= cliff_judge_ts) {
        ///< 颠簸状态中，之前保存的旧值无效
        if (is_bump_detected) {
            is_dist_old_save = false;
        }

        if (cliff_data.dist > sys_paras.luna_dis_normal && dist_old > sys_paras.luna_dis_normal && (cliff_data.dist > dist_old) &&
            ((cliff_data.dist - dist_old) >= sys_paras.luna_delt_threshold) && is_dist_old_save) {
            if (osTimerIsRunning(cliff_judge_timer) == 0) {
                osTimerStart(cliff_judge_timer, sys_paras.imu_bump_smp_inval);
            }
        }

        dist_old = cliff_data.dist;

        is_dist_old_save = true;

        cliff_judge_ts = osKernelGetTickCount() + sys_paras.luna_smp_inval;
    }

    ///< 开机 2秒后，检测到超短测距，认为是手动遮挡单点激光，清除断崖检测
    if (cliff_data.dist < sys_paras.luna_dis_min && fal_pmu_sys_up_time() > 2000) {
        log_i("cliff detect cliff clear: too short[%d] cm", cliff_data.dist);

        sys_exc_clear(EXC66_CLIFF);

        is_dist_old_save = false;
    }

    ///< 开机后的 2至5秒，检测到长测距，认为开机时前面是断崖
    if (cliff_data.dist > sys_paras.luna_dis_max && fal_pmu_sys_up_time() > 2000 && fal_pmu_sys_up_time() < 5000) {
        log_i("cliff detect cliff set: too long at system on time. [%d] cm", cliff_data.dist);

        sys_exc_set(EXC66_CLIFF);

        is_dist_old_save = false;
    }
}

#define CLIFF_INDEX_HEADER0  0X00
#define CLIFF_INDEX_HEADER1  0X01
#define CLIFF_INDEX_CHECKSUM 0X08

#define CLIFF_FRAME_LEN_MAX 9
#define CLIFF_FRAME_HEADER0 0X59
#define CLIFF_FRAME_HEADER1 0X59

//< 断崖检测 数据处理
void cliff_data_handle(uint8_t data) {
    static uint8_t frame_buff[CLIFF_FRAME_LEN_MAX];
    static uint8_t buff_index = 0;
    ;
    static uint8_t rec_sta = 0;

    CLIFF_FRMAE_DATA_P rec_frame = (CLIFF_FRMAE_DATA_P) frame_buff;

    uint8_t check_sum = 0;

    switch (rec_sta) {
        case 0:

            frame_buff[buff_index++] = data;
            ;

            if (buff_index == CLIFF_INDEX_HEADER0 + 1) {
                if (rec_frame->header0 == CLIFF_FRAME_HEADER0) {
                    rec_sta = 1;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 1:

            frame_buff[buff_index++] = data;
            ;

            if (buff_index == CLIFF_INDEX_HEADER1 + 1) {
                if (rec_frame->header1 == CLIFF_FRAME_HEADER1) {
                    rec_sta = 2;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 2:
            frame_buff[buff_index++] = data;

            if (buff_index == CLIFF_INDEX_CHECKSUM + 1) {
                check_sum = cliff_frame_checksum(frame_buff, sizeof(CLIFF_FRMAE_DATA_T) - 1);

                if (rec_frame->checksum == check_sum) {
                    log_d(
                        "cliff data receive success. dist[%d]cm, amp[%d], "
                        "temp[%d] "
                        "C",
                        rec_frame->dist, rec_frame->amp, rec_frame->temp / 8 - 256);

                    // cliff_data = *rec_frame;
                    cliff_data_decode_callback(rec_frame);
                } else {
                    log_d(
                        "cliff data receive checksum error, want[%#06X], "
                        "get[%#06X].",
                        rec_frame->checksum, check_sum);
                }

                buff_index = 0;
                rec_sta    = 0;
            }

            break;

        default:
            buff_index = 0;
            rec_sta    = 0;
            break;
    }
}

static uint8_t cliff_frame_checksum(uint8_t *data, uint16_t len) {
    uint8_t checksum = 0;
    uint8_t i;

    for (i = 0; i < len; i++) {
        checksum += data[i];
    }

    return checksum;
}
#if 0
static void fal_cliff_data_public(void)
{
	uros_cliff.radiation_type = 3;
	uros_cliff.min_range = 0.1;
	uros_cliff.max_range = 5.0;
	uros_cliff.range = 1.23;
	pal_uros_msg_set_timestamp(&uros_cliff.header.stamp);
	message_publish(&g_uros_cliff);
}
#endif
#ifdef __cplusplus
}
#endif

/* @} FAL_CLIFF */
/* @} Robot_FAL */
