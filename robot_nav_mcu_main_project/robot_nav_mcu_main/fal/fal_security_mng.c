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
#include "fal_security_mng.h"
#include "fal_usound.h"
#include "cmsis_os.h"
#include "lwrb.h"
#include "usart.h"
#include "shell.h"
#include "sys_exc.h"

#define LOG_TAG "fal_secur"
#include "elog.h"

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_SECURITY 安保任务板管理功能 - FAL
 *
 * @brief 主要是接收安保 MCU103 的超声测距数据和对其反馈开关机状态处理\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/
#define SECURITY_RECV_BUFF_SIZE 256

#define SECUR_LIFE_EXTEND 1000

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
uint8_t security_recv_byte;
lwrb_t  security_rbuff;

osSemaphoreId_t security_empty_id  = NULL;
osSemaphoreId_t security_filled_id = NULL;

TASK_USOUND_DATA_T task_usound_data = {0};

extern osMutexId_t mutex_usound_data;

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/
static uint8_t security_recv_buff[SECURITY_RECV_BUFF_SIZE];

static uint32_t secur_live_ts = 0;

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void           fal_security_measure_on(void);
void           security_data_handle(uint8_t data);
static uint8_t security_frame_checksum(uint8_t *data, uint16_t len);
extern void    task_soc_sta_set(uint8_t sta);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_security_mng_init
 * Description:    初始化 fal_security_mng_init
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
int fal_security_mng_init(void) {
    /*添加模块处理函数*/
    lwrb_init(&security_rbuff, security_recv_buff, sizeof(security_recv_buff));

    security_empty_id  = osSemaphoreNew(SECURITY_RECV_BUFF_SIZE, SECURITY_RECV_BUFF_SIZE, NULL);
    security_filled_id = osSemaphoreNew(SECURITY_RECV_BUFF_SIZE, 0U, NULL);

    HAL_UART_Receive_IT(&SECURITY_USART_H, (uint8_t *) &security_recv_byte, 1);

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_deinit
 * Description:    释放 fal_security_mng 层资源
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
int fal_security_mng_deInit(void) {
    return 0;
}

void task_fal_security_mng_run(void *argument) {
    uint8_t data;

    fal_security_mng_init();

    secur_live_ts = osKernelGetTickCount() + SECUR_LIFE_EXTEND;

    for (;;) {
        if (READ_BIT(usound_flag, NAV_CMD_BIT)) {
            CLEAR_BIT(usound_flag, NAV_CMD_BIT);
            //发送检测任务超声
            fal_security_measure_on();
            log_d("task usound meature");
        }

        if (osSemaphoreAcquire(security_filled_id, 100) == osOK) {
            // consume data
            if (lwrb_read(&security_rbuff, &data, 1)) {
                security_data_handle(data);
            }

            osSemaphoreRelease(security_empty_id);
        }

        if (osKernelGetTickCount() >= secur_live_ts) {
            sys_exc_set(EXC36_TASK_SECUR_TO);
        } else {
            sys_exc_clear(EXC36_TASK_SECUR_TO);
        }
    }
}

#define SECURITY_INDEX_HEADER 0X00
#define SECURITY_INDEX_LEN    0X01
#define SECURITY_INDEX_CMD    0X02
#define SECURITY_INDEX_DATA   0X03
//#define SECURITY_INDEX_CHECKSUM             ///< 变长数据，此处不宏固定
uint8_t security_index_checksum;

#define SECURITY_FRAME_DATA_LEN_MAX 17
#define SECURITY_FRAME_HEADER       0X2E

#define SECURITY_FRAME_UP_RESP     0X01
#define SECURITY_FRAME_DOWN_RESP   0X02
#define SECURITY_FRAME_USOUND_DATA 0X03

#define SECURITY_OK      0X01
#define SECURITY_TIMEOUT 0X02

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t header;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[SECURITY_FRAME_DATA_LEN_MAX - 1];  ///< 减去 CMD
    uint8_t checksum;

} SECURITY_FRMAE_DATA_T, *SECURITY_FRMAE_DATA_P;

#pragma pack(pop)

//< 安保任务103 数据处理
void security_data_handle(uint8_t data) {
    static uint8_t frame_buff[SECURITY_FRAME_DATA_LEN_MAX + 3];  ///< 加上 HEADER CMD CHECKSUM
    static uint8_t buff_index = 0;
    ;
    static uint8_t rec_sta = 0;

    SECURITY_FRMAE_DATA_P rec_frame = (SECURITY_FRMAE_DATA_P) frame_buff;

    uint8_t check_sum = 0;

    if (buff_index >= sizeof(frame_buff)) {
        buff_index = 0;
        rec_sta    = 0;
    }

    switch (rec_sta) {
        case 0:

            frame_buff[buff_index++] = data;
            ;

            if (buff_index == SECURITY_INDEX_HEADER + 1) {
                if (rec_frame->header == SECURITY_FRAME_HEADER) {
                    rec_sta = 1;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 1:

            frame_buff[buff_index++] = data;

            if (buff_index == SECURITY_INDEX_LEN + 1) {
                if (rec_frame->len <= SECURITY_FRAME_DATA_LEN_MAX) {
                    security_index_checksum = rec_frame->len + 2;
                    rec_sta                 = 2;
                } else {
                    buff_index = 0;
                    rec_sta    = 0;
                }
            }

            break;

        case 2:
            frame_buff[buff_index++] = data;

            if (buff_index == SECURITY_INDEX_CMD + 1) {
                rec_sta = 4;
            }

            break;

            /*跳过此项，状态4中等待校验字节就行
            case 3:

                frame_buff[buff_index++] = data;

                if(buff_index == SECURITY_INDEX_DATA+1)
                {
                    rec_sta = 4;
                }

                break;
            */

        case 4:
            frame_buff[buff_index++] = data;

            if (buff_index == security_index_checksum + 1) {
                check_sum = security_frame_checksum(frame_buff, rec_frame->len + 2);
                if (frame_buff[security_index_checksum] == check_sum) {
                    if (rec_frame->cmd == SECURITY_FRAME_UP_RESP) {
                        if (rec_frame->data[0] == SECURITY_OK) {
                            log_i("task103 resp up success.");
                        } else if (rec_frame->data[0] == SECURITY_TIMEOUT) {
                            log_i("task103 resp up timeout.");
                        }
                        task_soc_sta_set(1);
                    } else if (rec_frame->cmd == SECURITY_FRAME_DOWN_RESP) {
                        if (rec_frame->data[0] == SECURITY_OK) {
                            log_i("task103 resp down success.");
                        } else if (rec_frame->data[0] == SECURITY_TIMEOUT) {
                            log_i("task103 resp down timeout.");
                        }
                        task_soc_sta_set(0);
                    } else if (rec_frame->cmd == SECURITY_FRAME_USOUND_DATA) {
                        secur_live_ts = osKernelGetTickCount() + SECUR_LIFE_EXTEND;

                        osMutexAcquire(mutex_usound_data, osWaitForever);

                        task_usound_data.dis1 = (rec_frame->data[0] << 8) + rec_frame->data[1];
                        task_usound_data.dis2 = (rec_frame->data[2] << 8) + rec_frame->data[3];

                        osMutexRelease(mutex_usound_data);

                        log_d("task usound %04d mm %04d mm", task_usound_data.dis1, task_usound_data.dis2);
                        SET_BIT(usound_flag, TASK_FINISH_BIT);
                    }

                    // log_i("security data receive success.");
                } else {
                    elog_hexdump("task usound", 32, frame_buff, buff_index);
                    log_e(
                        "security data receive checksum error, want[%#06X], "
                        "get[%#06X]. frame len[%d]",
                        frame_buff[buff_index - 1], check_sum, buff_index);
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

void fal_security_measure_on(void) {
    uint8_t buff[4];
    buff[0] = 0X2E;
    buff[1] = 0X01;
    buff[2] = 0X04;
    buff[3] = buff[0] + buff[1] + buff[2];

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    if (osKernelGetState() == osKernelRunning) {
        osDelay(10);
    } else {
        HAL_Delay(10);
    }
    HAL_UART_Transmit(&SECURITY_USART_H, buff, 4, 100);
    if (osKernelGetState() == osKernelRunning) {
        osDelay(10);
    } else {
        HAL_Delay(10);
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), task103_measure_on, fal_security_measure_on,
                 fal_security_measure_on);

void fal_security_power_on(void) {
    uint8_t buff[4];
    buff[0] = 0X2E;
    buff[1] = 0X01;
    buff[2] = 0X01;
    buff[3] = buff[0] + buff[1] + buff[2];

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(10);
    HAL_UART_Transmit(&SECURITY_USART_H, buff, 4, 100);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), task103_power_on, fal_security_power_on, fal_security_power_on);

void fal_security_power_off(void) {
    uint8_t buff[4];
    buff[0] = 0X2E;
    buff[1] = 0X01;
    buff[2] = 0X02;
    buff[3] = buff[0] + buff[1] + buff[2];

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(10);
    HAL_UART_Transmit(&SECURITY_USART_H, buff, 4, 100);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), task103_power_off, fal_security_power_off,
                 fal_security_power_off);

static uint8_t security_frame_checksum(uint8_t *data, uint16_t len) {
    uint8_t checksum = 0;
    uint8_t i;

    for (i = 0; i < len; i++) {
        checksum += data[i];
    }

    return checksum;
}

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_407 */
