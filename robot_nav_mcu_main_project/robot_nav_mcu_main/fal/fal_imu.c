/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-25
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
#include "fal_imu.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "usart.h"
#include "lwrb.h"
#include "math.h"
#include "bsp_imu.h"
#include "sys_pubsub.h"
#include "sys_paras.h"
#include "pal_uros.h"
#include "sensor_msgs/msg/imu.h"

#define LOG_TAG "fal_imu"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_IMU - IMU
 *
 * @brief  \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define IMU_RECV_BUFF_SIZE 1024

#define IMU_LIVE_TIME_MS 1000

#define PI 3.1415926f

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
uint8_t imu_recv_byte;
lwrb_t  imu_rbuff;

osSemaphoreId_t imu_empty_id  = NULL;
osSemaphoreId_t imu_filled_id = NULL;

osMutexId_t mutex_imu_data = NULL;

const osMutexAttr_t mutex_imu_data_attr = {
    "mutex_imu_data",                       // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static uint8_t imu_recv_buff[IMU_RECV_BUFF_SIZE];
IMU_DATA_T     imu_sensor_data = {0};

static uint32_t imu_live_ts = 0;

static osTimerId_t bump_detect_to_timer = NULL;

static bool pitch_old_save = false;

static publisher             g_uros_imu;
static sensor_msgs__msg__Imu uros_imu;
static char                  imu_buffer[10];

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void imu_data_handle(uint8_t data);
static void bump_detect_to_cb(void *argument);
static void fal_imu_data_public(void);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_imu_init
 * Description:    初始化 fal_imu
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示打开文件成功
 *  - 1 表示打开文件失败
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
int fal_imu_init(void) {
    /*添加模块处理函数*/
    /*
    while(1)
    {
        if(bsp_imu_init() == 0)
        {
            sys_exc_clear(EXC39_IMU_DATA);

            imu_live_ts = osKernelGetTickCount() + IMU_LIVE_TIME_MS;
            break;
        }
        else
        {
            log_e("IMU init error, reiniting...");

            sys_exc_set(EXC39_IMU_DATA);

            osDelay(1000);
        }
    }
    */
    publisher_init(&g_uros_imu, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu), "/imu", &uros_imu, BEST, OVERWRITE,
                   sizeof(sensor_msgs__msg__Imu));

    uros_imu.header.frame_id.data     = imu_buffer;
    uros_imu.header.frame_id.capacity = 10;
    sprintf(uros_imu.header.frame_id.data, "imu");

    lwrb_init(&imu_rbuff, imu_recv_buff, sizeof(imu_recv_buff));

    bump_detect_to_timer = osTimerNew(bump_detect_to_cb, osTimerOnce, NULL, NULL);

    imu_empty_id  = osSemaphoreNew(IMU_RECV_BUFF_SIZE, IMU_RECV_BUFF_SIZE, NULL);
    imu_filled_id = osSemaphoreNew(IMU_RECV_BUFF_SIZE, 0U, NULL);

    mutex_imu_data = osMutexNew(&mutex_imu_data_attr);

#if 0
    HAL_UART_Receive_IT(&IMU_USART_H, (uint8_t *) &imu_recv_byte, 1);
#endif

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_shell_deInit
 * Description:    释放 fal_shell 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
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
int fal_imu_deInit(void) {
    return 0;
}

void task_fal_imu_run(void *argument) {
    fal_imu_init();
#if 0
    uint8_t data;
#else
    size_t dma_head = 0;
    size_t dma_tail = 0;
#endif
    uint32_t print_ts = 0;

    for (;;) {
#if 0
        if (osSemaphoreAcquire(imu_filled_id, 100) == osOK) {
            // consume data
            if (lwrb_read(&imu_rbuff, &data, 1)) {
                imu_data_handle(data);
            }

            osSemaphoreRelease(imu_empty_id);
        }
#else
        if (dma_head == dma_tail) {
            osSemaphoreAcquire(imu_filled_id, 100);
        }

        dma_tail = IMU_RECV_BUFF_SIZE - __HAL_DMA_GET_COUNTER(IMU_USART_H.hdmarx);

        size_t wrote = 0;
        size_t len   = dma_tail - dma_head;

        while ((dma_head != dma_tail) && (wrote < len)) {
            imu_data_handle(imu_recv_buff[dma_head]);
            dma_head = (dma_head + 1) % IMU_RECV_BUFF_SIZE;
            wrote++;
        }
#endif
        if (osKernelGetTickCount() >= print_ts) {
            log_i("IMU Roll[%.2f] Pitch[%.2f] Yaw[%.2f]", imu_sensor_data.euler_roll, imu_sensor_data.euler_pitch, imu_sensor_data.euler_yaw);

            print_ts = osKernelGetTickCount() + 1000;
        }

        if (osKernelGetTickCount() >= imu_live_ts && fal_pmu_get_sta() != PMU_STA_STANDBY) {
            log_w("IMU lost, reseting...");

            sys_exc_set(EXC39_IMU_DATA);

            pitch_old_save = false;

            bsp_imu_reset();

            osDelay(100);

#if 0
            HAL_UART_AbortReceive_IT(&IMU_USART_H);
#else
            __HAL_UART_DISABLE_IT(&IMU_USART_H, UART_IT_IDLE);
            HAL_UART_DMAStop(&IMU_USART_H);
#endif
            while (1) {
                if (bsp_imu_init() == 0) {
                    sys_exc_clear(EXC39_IMU_DATA);

                    imu_live_ts = osKernelGetTickCount() + IMU_LIVE_TIME_MS;
                    break;
                } else {
                    osDelay(1000);
                }
            }
#if 0
            HAL_UART_Receive_IT(&IMU_USART_H, (uint8_t *) &imu_recv_byte, 1);
#else
            __HAL_UART_ENABLE_IT(&IMU_USART_H, UART_IT_IDLE);
            HAL_UART_Receive_DMA(&IMU_USART_H, imu_recv_buff, IMU_RECV_BUFF_SIZE);
#endif
        }
    }
}

static void bump_detect_to_cb(void *argument) {
    log_i("imu detect bump clear!");

    BUMP_DETECT_T bump_detect    = {0};
    bump_detect.is_bump_detected = false;
    pub_topic(SYS_EVT_BUMP_DETECT, &bump_detect);
}

static void imu_data_decode_callback(IMU_DATA_P data) {
    static uint32_t imu_bump_judge_ts = 0;
    static float    pitch_old;

    BUMP_DETECT_T bump_detect = {0};

    osMutexAcquire(mutex_imu_data, osWaitForever);

    imu_sensor_data = *data;

    osMutexRelease(mutex_imu_data);

    fal_imu_data_public();

    log_d("IMU Roll[%.2f] Pitch[%.2f] Yaw[%.2f]", imu_sensor_data.euler_roll, imu_sensor_data.euler_pitch, imu_sensor_data.euler_yaw);

    if (osKernelGetTickCount() >= imu_bump_judge_ts) {
        if (((fabs(imu_sensor_data.euler_pitch - pitch_old) * 10) >= sys_paras.imu_pitch_threshold) && pitch_old_save) {
            log_i("imu detect bump set!");

            bump_detect.is_bump_detected = true;
            pub_topic(SYS_EVT_BUMP_DETECT, &bump_detect);

            osTimerStart(bump_detect_to_timer, sys_paras.bump_up_hold_time_ms);
        }

        pitch_old = imu_sensor_data.euler_pitch;

        pitch_old_save = true;

        imu_bump_judge_ts = osKernelGetTickCount() + sys_paras.imu_bump_smp_inval;
    }
}

//< IMU 数据处理
static void imu_data_handle(uint8_t data) {
    static uint8_t frame_buff[IMU_FRAME_LEN_MAX];
    static uint8_t buff_index = 0;
    static uint8_t rec_sta    = 0;

    IMU_FRMAE_DATA_P rec_frame = (IMU_FRMAE_DATA_P) frame_buff;

    uint16_t check_sum = 0;

    switch (rec_sta) {
        case 0:

            frame_buff[buff_index++] = data;

            if (buff_index == IMU_INDEX_HEADER + 1) {
                if (rec_frame->header == IMU_FRAME_HEADER) {
                    rec_sta = 1;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 1:

            frame_buff[buff_index++] = data;

            if (buff_index == IMU_INDEX_SENSOR_H + 1) {
                if (rec_frame->sensor_id == IMU_ADDR) {
                    rec_sta = 2;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 2:
            frame_buff[buff_index++] = data;

            if (buff_index == IMU_INDEX_CMD_H + 1) {
                rec_sta = 3;
            }

            break;

        case 3:

            frame_buff[buff_index++] = data;

            if (buff_index == IMU_INDEX_LEN_H + 1) {
                if (rec_frame->data_len <= IMU_FRAME_LEN_MAX - 11) {
                    rec_sta = 4;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 4:
            frame_buff[buff_index++] = data;

            if (buff_index == rec_frame->data_len + 11) {
                check_sum = bsp_imu_frame_check_sum((uint8_t *) &rec_frame->sensor_id, rec_frame->data_len + 6);

                if (rec_frame->lrc == check_sum) {
                    imu_data_decode_callback(&rec_frame->data);

                    /*
                    osMutexAcquire(mutex_imu_data, osWaitForever);
                    imu_sensor_data = rec_frame->data;
                    osMutexRelease(mutex_imu_data);
                    */

                    // log_i("IMU data receive success, %d",
                    // rec_frame->data.ts);

                    imu_live_ts = osKernelGetTickCount() + IMU_LIVE_TIME_MS;

                } else {
                    log_e("IMU data receive lrc error, want[%#06X], get[%#06X].", rec_frame->lrc, check_sum);
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

static void fal_imu_data_public(void) {
    osMutexAcquire(mutex_imu_data, osWaitForever);

    uros_imu.orientation.w         = imu_sensor_data.quat_w;
    uros_imu.orientation.x         = imu_sensor_data.quat_x * (-1.0);
    uros_imu.orientation.y         = imu_sensor_data.quat_y * (-1.0);
    uros_imu.orientation.z         = imu_sensor_data.quat_z * (-1.0);
    uros_imu.angular_velocity.x    = imu_sensor_data.gyro_x * PI / 180.0f;
    uros_imu.angular_velocity.y    = imu_sensor_data.gyro_y * PI / 180.0f;
    uros_imu.angular_velocity.z    = imu_sensor_data.gyro_z * PI / 180.0f;
    uros_imu.linear_acceleration.x = imu_sensor_data.acc_x;
    uros_imu.linear_acceleration.y = imu_sensor_data.acc_y;
    uros_imu.linear_acceleration.z = imu_sensor_data.acc_z;

    osMutexRelease(mutex_imu_data);

    pal_uros_msg_set_timestamp(&uros_imu.header.stamp);
    message_publish(&g_uros_imu);
}

#ifdef __cplusplus
}
#endif

/* @} FAL_IMU */
/* @} Robot_FAL */
