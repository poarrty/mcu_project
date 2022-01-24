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
#include "fal_usound.h"
#include "cmsis_os.h"
#include "lwrb.h"
#include "usart.h"
#include "shell.h"
#include "fal_pmu.h"
#include "pal_uros.h"
#include "fal_ota.h"
#include "crc16.h"
#include "sensor_msgs/msg/range.h"

#define LOG_TAG "fal_usound"
#include "elog.h"
/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_USOUND  - USOUND
 *
 * @brief 主要是接收处理底盘 MCU103 的超声测距数据\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define USOUND_RECV_BUFF_SIZE 256

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
char     usound_version[10];
uint32_t fileoffest;
uint8_t  datalen;
uint8_t  is_transdata;

uint8_t usound_flag     = 0;
uint8_t nav_reset_count = 0;

uint8_t usound_recv_byte;
lwrb_t  usound_rbuff;

osSemaphoreId_t usound_empty_id  = NULL;
osSemaphoreId_t usound_filled_id = NULL;

NVG_USOUND_DATA_T nvg_usound_data = {0};

osMutexId_t mutex_usound_data = NULL;

const osMutexAttr_t mutex_usound_data_attr = {
    "mutex_usound_data",                    // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
static uint8_t                 usound_recv_buff[USOUND_RECV_BUFF_SIZE];
static publisher               g_uros_usound;
static sensor_msgs__msg__Range uros_usound;
static char                    usound_buffer[10];

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void        fal_nav_usound_measure_on(void);
void        usound_data_handle(uint8_t data);
uint8_t     usound_frame_checksum(uint8_t *data, uint16_t len);
static void fal_usound_data_public(uint16_t data, uint8_t usound_id);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_usound_init
 * Description:    初始化 fal_usound_init
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
int fal_usound_init(void) {
    publisher_init(&g_uros_usound, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range), "/usound", &uros_usound, BEST, OVERWRITE,
                   sizeof(sensor_msgs__msg__Range));

    uros_usound.header.frame_id.data     = usound_buffer;
    uros_usound.header.frame_id.capacity = sizeof(usound_buffer);

    /*添加模块处理函数*/
    lwrb_init(&usound_rbuff, usound_recv_buff, sizeof(usound_recv_buff));

    usound_empty_id  = osSemaphoreNew(USOUND_RECV_BUFF_SIZE, USOUND_RECV_BUFF_SIZE, NULL);
    usound_filled_id = osSemaphoreNew(USOUND_RECV_BUFF_SIZE, 0U, NULL);

    mutex_usound_data = osMutexNew(&mutex_usound_data_attr);

    HAL_UART_Receive_IT(&USOUND_USART_H, (uint8_t *) &usound_recv_byte, 1);

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_usound_deInit
 * Description:    释放 fal_usound 资源
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
int fal_usound_deInit(void) {
    return 0;
}

void task_fal_usound_run(void *argument) {
    uint8_t  data;
    uint32_t nav_timeout = 0;

    fal_usound_init();

    for (;;) {
        if (usound_updating_info.is_finished == 0) {
            if (((!READ_BIT(usound_flag, NAV_FINISH_BIT)) || (!READ_BIT(usound_flag, TASK_FINISH_BIT))) &&
                (osKernelGetTickCount() >= nav_timeout)) {
                if (fal_pmu_get_sta() == PMU_STA_RUNNING) {
                    log_w("usound timeout, restart");
                    SET_BIT(usound_flag, NAV_FINISH_BIT | TASK_FINISH_BIT);
                }
            }

            if (READ_BIT(usound_flag, NAV_FINISH_BIT) && READ_BIT(usound_flag, TASK_FINISH_BIT)) {
                CLEAR_BIT(usound_flag, NAV_FINISH_BIT | TASK_FINISH_BIT);
                nav_timeout = osKernelGetTickCount() + 500;
                //发送检测底盘超声
                fal_nav_usound_measure_on();
                if (nav_reset_count++ >= 5) {
                    log_e("---nav usound power reset---");
                    nav_reset_count = 0;
                    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_RESET);
                    osDelay(500);
                    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_SET);
                }
            }
        }

        if (osSemaphoreAcquire(usound_filled_id, 100) == osOK) {
            // consume data
            if (lwrb_read(&usound_rbuff, &data, 1)) {
                usound_data_handle(data);
            }

            osSemaphoreRelease(usound_empty_id);
        }
    }
}

#define USOUND_INDEX_HEADER 0X00
#define USOUND_INDEX_LEN    0X01
#define USOUND_INDEX_CMD    0X02

//#define SECURITY_INDEX_CHECKSUM             ///< 变长数据，此处不宏固定
uint8_t usound_index_checksum;

#define USOUND_FRAME_DATA_LEN_MAX 255
#define USOUND_FRAME_HEADER       0X55
#define USOUND_FRAME_NAV_DATA     0X03
#define USOUND_FRAME_NAV_FINISH   0X05
#define USOUND_FRAME_SOFT_VERSION 0X06
#define USOUND_FRAME_OTA          0X07
typedef struct {
    uint8_t header;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[USOUND_FRAME_DATA_LEN_MAX - 1];  ///< 减去 CMD
    uint8_t checksum;

} USOUND_FRMAE_DATA_T, *USOUND_FRMAE_DATA_P;

//< 底盘超声测距数据处理
void usound_data_handle(uint8_t data) {
    static uint8_t   frame_buff[USOUND_FRAME_DATA_LEN_MAX + 3];  ///< 加上 HEADER CMD CHECKSUM
    static uint8_t   buff_index_x = 0;
    static uint8_t   rec_sta      = 0;
    static uint16_t *pDis         = NULL;

    USOUND_FRMAE_DATA_P rec_frame = (USOUND_FRMAE_DATA_P) frame_buff;

    uint8_t check_sum = 0;

    if (buff_index_x >= sizeof(frame_buff)) {
        buff_index_x = 0;
        rec_sta      = 0;
    }

    switch (rec_sta) {
        case 0:
            memset(rec_frame->data, 0, sizeof(rec_frame->data));
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == USOUND_INDEX_HEADER + 1) {
                if (rec_frame->header == USOUND_FRAME_HEADER) {
                    rec_sta = 1;
                } else {
                    buff_index_x = 0;
                    rec_sta      = 0;
                }
            }

            break;

        case 1:
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == USOUND_INDEX_LEN + 1) {
                if (rec_frame->len <= USOUND_FRAME_DATA_LEN_MAX) {
                    usound_index_checksum = rec_frame->len + 2;
                    rec_sta               = 2;
                } else {
                    buff_index_x = 0;
                    rec_sta      = 0;
                }
            }

            break;

        case 2:
            frame_buff[buff_index_x++] = data;
            if (data == USOUND_FRAME_NAV_FINISH) {
                rec_sta = 3;
            } else if (data == USOUND_FRAME_NAV_DATA) {
                rec_sta = 5;
            } else if (data == USOUND_FRAME_SOFT_VERSION) {
                rec_sta = 6;
            } else if (data == USOUND_FRAME_OTA) {
                rec_sta = 7;
            }
            break;

        /*超声分时解析*/
        case 3:
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == usound_index_checksum + 1) {
                check_sum = usound_frame_checksum(&frame_buff[0], rec_frame->len + 2);
                if (frame_buff[usound_index_checksum] == check_sum) {
                    SET_BIT(usound_flag, NAV_CMD_BIT);
                } else {
                    log_e(
                        "NVG usound cmd receive lrc error, want[%#06X], "
                        "get[%#06X].",
                        frame_buff[buff_index_x - 1], check_sum);
                }

                buff_index_x = 0;
                rec_sta      = 0;
            }
            break;
        /*超声数据解析*/
        case 5:
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == usound_index_checksum + 1) {
                check_sum = usound_frame_checksum(&frame_buff[0], rec_frame->len + 2);
                if (frame_buff[usound_index_checksum] == check_sum) {
                    osMutexAcquire(mutex_usound_data, osWaitForever);

                    nvg_usound_data.dis1 = (rec_frame->data[0] << 8) + rec_frame->data[1];
                    nvg_usound_data.dis2 = (rec_frame->data[2] << 8) + rec_frame->data[3];
                    nvg_usound_data.dis3 = (rec_frame->data[4] << 8) + rec_frame->data[5];
                    nvg_usound_data.dis4 = (rec_frame->data[6] << 8) + rec_frame->data[7];
                    nvg_usound_data.dis5 = (rec_frame->data[8] << 8) + rec_frame->data[9];
                    nvg_usound_data.dis6 = (rec_frame->data[10] << 8) + rec_frame->data[11];
                    nvg_usound_data.dis7 = (rec_frame->data[12] << 8) + rec_frame->data[13];
                    nvg_usound_data.dis8 = (rec_frame->data[14] << 8) + rec_frame->data[15];

                    osMutexRelease(mutex_usound_data);

                    log_d(
                        "NVG usound/mm: %04d %04d %04d %04d %04d %04d %04d "
                        "%04d",
                        nvg_usound_data.dis1, nvg_usound_data.dis2, nvg_usound_data.dis3, nvg_usound_data.dis4, nvg_usound_data.dis5,
                        nvg_usound_data.dis6, nvg_usound_data.dis7, nvg_usound_data.dis8);

                    SET_BIT(usound_flag, NAV_FINISH_BIT);
                    nav_reset_count = 0;

                    pDis = &nvg_usound_data.dis1;
                    for (int i = 0; i < 8; i++) {
                        fal_usound_data_public(*pDis, i + 1);
                        osDelay(10);  // 临时解决topic丢包问题
                        pDis++;
                    }
                    pDis = NULL;

                    // log_i("NVG usound data receive success.");
                } else {
                    log_e(
                        "NVG usound data receive lrc error, want[%#06X], "
                        "get[%#06X].",
                        frame_buff[buff_index_x - 1], check_sum);
                }

                buff_index_x = 0;
                rec_sta      = 0;
            }

            break;
        /*超声版本号解析*/
        case 6:
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == usound_index_checksum + 1) {
                check_sum = usound_frame_checksum(&frame_buff[0], rec_frame->len + 2);
                if (frame_buff[usound_index_checksum] == check_sum) {
                    uint16_t data_crc16 = (uint16_t)(rec_frame->data[rec_frame->len - 3] << 8) + (uint16_t) rec_frame->data[rec_frame->len - 2];
                    if (data_crc16 == calculate_crc16(rec_frame->data, rec_frame->len - 3)) {
                        if ((strlen((const char *) rec_frame->data) > 2)) {
                            snprintf(usound_version, strlen((const char *) rec_frame->data) - 2, "%s", rec_frame->data);
                            log_i("nav_software %s", usound_version);
                        }
                    } else {
                        log_e(
                            "NVG usound data receive crc error, want[%#06X], "
                            "get[%#06X].",
                            calculate_crc16(rec_frame->data, rec_frame->len - 1), data_crc16);
                    }
                } else {
                    log_e(
                        "NVG usound cmd receive lrc error, want[%#06X], "
                        "get[%#06X].",
                        frame_buff[buff_index_x - 1], check_sum);
                }

                buff_index_x = 0;
                rec_sta      = 0;
            }
            break;

        case 7:
            frame_buff[buff_index_x++] = data;

            if (buff_index_x == usound_index_checksum + 1) {
                check_sum = usound_frame_checksum(&frame_buff[0], rec_frame->len + 2);
                if (frame_buff[usound_index_checksum] == check_sum) {
                    uint16_t data_crc16 = (uint16_t)(rec_frame->data[rec_frame->len - 3] << 8) + (uint16_t) rec_frame->data[rec_frame->len - 2];
                    if (data_crc16 == calculate_crc16(rec_frame->data, rec_frame->len - 3)) {
                        fileoffest = (uint32_t)(rec_frame->data[0] << 24) + (uint32_t)(rec_frame->data[1] << 16) +
                                     (uint32_t)(rec_frame->data[2] << 8) + rec_frame->data[3];
                        datalen      = rec_frame->data[4];
                        is_transdata = 1;
                    } else {
                        log_e(
                            "NVG usound data receive crc error, want[%#06X], "
                            "get[%#06X].",
                            calculate_crc16(rec_frame->data, rec_frame->len - 1), data_crc16);
                    }
                } else {
                    log_e(
                        "NVG usound cmd receive lrc error, want[%#06X], "
                        "get[%#06X].",
                        frame_buff[buff_index_x - 1], check_sum);
                }

                buff_index_x = 0;
                rec_sta      = 0;
            }
            break;

        default:
            buff_index_x = 0;
            rec_sta      = 0;
            break;
    }
}

void fal_nav_usound_measure_on(void) {
    uint8_t buff[4];
    buff[0] = 0X55;
    buff[1] = 0X01;
    buff[2] = 0X04;
    buff[3] = buff[0] + buff[1] + buff[2];

    HAL_UART_Transmit(&USOUND_USART_H, buff, 4, 1000);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nav103_measure_on, fal_nav_usound_measure_on,
                 fal_nav_usound_measure_on);

static void fal_usound_data_public(uint16_t data, uint8_t usound_id) {
    uros_usound.radiation_type = 0;
    uros_usound.min_range      = 0.13;
    uros_usound.max_range      = 1.4;
    /* 弧度制 */
    uros_usound.field_of_view = 0.959931;
    uros_usound.range         = (float) (data / 1000.0);
    pal_uros_msg_set_timestamp(&uros_usound.header.stamp);
    sprintf(uros_usound.header.frame_id.data, "usound_%d", usound_id);
    message_publish(&g_uros_usound);
}

uint8_t usound_frame_checksum(uint8_t *data, uint16_t len) {
    uint8_t  checksum = 0;
    uint16_t i;

    for (i = 0; i < len; i++) {
        checksum += data[i];
    }

    return checksum;
}

#ifdef __cplusplus
}
#endif

/* @} FAL_USOUND */
/* @} Robot_FAL */
