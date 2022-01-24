/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-19
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-5-19       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_update.h"
#include "cmsis_os.h"
#include "shell.h"
#include "stdio.h"
#include "wwdg.h"
#include "elog.h"
#include "fal_ota.h"
#include "rtt_fal.h"
#include "crc16.h"
#include "fal_usound.h"
#include "fal_pmu.h"

#include "flashdb.h"

unsigned char          y_update_flag = 1;
extern struct fdb_kvdb kvdb;

/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup FAL_UPDATE 固件升级 - update
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/
void fal_update_go(void);
void fal_usound_update_go(void);
void fal_usound_update_data(uint32_t offset, uint8_t len);
/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_update_init
 * Description:    初始化 fal_update
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
int fal_update_init(void) {
    fal_ota_init();
    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_update_deInit
 * Description:    释放 fal_update 资源
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
int fal_update_deInit(void) {
    return 0;
}

uint8_t sfb_head[512];

void task_fal_update_run(void *argument) {
    uint8_t  is_usound_update_start = 0;
    uint8_t  usound_update_errorcnt = 0;
    uint32_t usound_update_timeout  = 0;
    fal_update_init();

    for (;;) {
#ifdef IAP
        /*主MCU升级*/
        if (main_updating_info.is_finished == 1) {
            const struct fal_partition *partition = NULL;

            partition = fal_partition_find("main_mcu_temp");

            if (partition == NULL) {
                log_e("Find partition (%s) failed!", "main_mcu_temp");
            } else {
                fal_partition_read(partition, 0, sfb_head, sizeof(sfb_head));
            }

            partition = fal_partition_find("boot_swap");

            if (partition == NULL) {
                log_e("Find partition (%s) failed!", "boot_swap");
            } else {
                taskENTER_CRITICAL();
                fal_partition_erase_all(partition);
                fal_partition_write(partition, 0, sfb_head, sizeof(sfb_head));
                taskEXIT_CRITICAL();
            }

            fal_update_go();
        }
#endif
        /*驱动器MCU升级*/
        if (motor_updating_info.is_finished == 1) {
        }
        /*超声MCU升级*/
        if (usound_updating_info.is_finished == 1) {
            if ((is_usound_update_start == 0) && (osKernelGetTickCount() >= usound_update_timeout)) {
                usound_update_timeout = osKernelGetTickCount() + 2000;
                if (usound_update_errorcnt++ >= 5) {
                    usound_update_errorcnt = 0;
                    log_w("---nav usound power reset---");
                    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_RESET);
                    osDelay(500);
                    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_SET);
                    osDelay(500);
                }
                log_i("start usound ota");
                fal_usound_update_go();
            }

            if (is_transdata == 1) {
                is_transdata           = 0;
                is_usound_update_start = 1;
                log_i("usound ota fileoffest = %d, datalen = %d", fileoffest, datalen);
                fal_usound_update_data(fileoffest, datalen);
            }

            if (osKernelGetTickCount() >= usound_updating_info.finish_time + 100000) {
                log_e("usound ota timeout");
                usound_updating_info.is_finished = 0;
                is_usound_update_start           = 0;
            }

            if ((fileoffest + datalen) >= usound_updating_info.total_size) {
                fileoffest = 0;
                datalen    = 0;
                log_i("usound_updating_info.total_size :%ld", usound_updating_info.total_size);
                is_usound_update_start = 0;
                usound_update_errorcnt = 0;
                /*等待超声启动，延迟10s并不影响其他升级*/
                osDelay(10000);
                log_i("------usound ota finish------");
                memset(&usound_updating_info, 0, sizeof(usound_updating_info));
            }
            goto otadelay;
        }
        osDelay(500);
    otadelay:
        osDelay(10);
    }
}

extern uint8_t usound_frame_checksum(uint8_t *data, uint16_t len);
void           fal_usound_update_go(void) {
    uint8_t buff[42];
    buff[0] = 0X55;
    buff[1] = 0X27;
    buff[2] = 0X06;
    buff[3] = usound_updating_info.total_size >> 24;
    buff[4] = usound_updating_info.total_size >> 16;
    buff[5] = usound_updating_info.total_size >> 8;
    buff[6] = usound_updating_info.total_size;
    memcpy(&buff[7], usound_updating_info.total_md5, 32);
    buff[39] = calculate_crc16(&buff[3], 36) >> 8;
    buff[40] = calculate_crc16(&buff[3], 36);
    buff[41] = usound_frame_checksum(buff, 41);

    HAL_UART_Transmit(&USOUND_USART_H, buff, 42, 1000);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fal_usound_update_go, fal_usound_update_go,
                 fal_usound_update_go);

void fal_usound_update_data(uint32_t offset, uint8_t len) {
    uint8_t buff[len + 6];
    buff[0] = 0X55;
    buff[1] = len + 3;
    buff[2] = 0X07;

    const struct fal_partition *partition = NULL;
    partition                             = fal_partition_find("usound_temp");

    if (partition == NULL) {
        log_e("Find partition (%s) failed!", "usound_temp");
    } else {
        fal_partition_read(partition, offset, &buff[3], len);
    }
    buff[len + 3] = calculate_crc16(&buff[3], len) >> 8;
    buff[len + 4] = calculate_crc16(&buff[3], len);
    buff[len + 5] = usound_frame_checksum(buff, len + 5);

    HAL_UART_Transmit(&USOUND_USART_H, buff, len + 6, 1000);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fal_usound_update_data, fal_usound_update_data,
                 fal_usound_update_data);

void fal_update_go(void) {
    /*
    #ifndef IAP_UPDATE
    osKernelLock();
    while(1);
    osKernelUnlock();
    #endif
    */
    log_i("----------update go----------");
    __set_FAULTMASK(1);      // 关闭所有中断
    HAL_NVIC_SystemReset();  // 复位
}

#include "usart.h"
#include "tim.h"
#define BOOTLOADER_ADDRESS (uint32_t) 0x08000000  ///< BOOT 启动地址
typedef void (*pFunction)(void);                  ///< 定义函数指针
pFunction JumpToApplication;                      ///< 跳转函数指针变量
uint32_t  JumpAddress;                            ///< 跳转地址

/*****************************************************************/
/**
 * Function: update_jump_boot
 * Description: 升级跳转函数，跳转至 Bootloader
 * Calls:
 * Called By:
 * @param[in]      None
 * @param[out]     None
 * @return
 *  - None
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
static void update_jump_boot(void) {
    if (((*(__IO uint32_t *) BOOTLOADER_ADDRESS) & 0x2FFE0000) == 0x20000000 ||
        ((*(__IO uint32_t *) BOOTLOADER_ADDRESS) & 0x2FFE0000) == 0x20020000) {
        /* Jump to user application */
        JumpAddress       = *(__IO uint32_t *) (BOOTLOADER_ADDRESS + 4);
        JumpToApplication = (pFunction) JumpAddress;

        struct fdb_blob blob;
        y_update_flag = 1;
        fdb_kv_set_blob(&kvdb, "y_update_flag", fdb_blob_make(&blob, (uint8_t *) &y_update_flag, sizeof(y_update_flag)));

        log_i("Stack Pointer[0X%lX] Jump Address[0X%lX] Control Word[0X%lX]", ((*(__IO uint32_t *) BOOTLOADER_ADDRESS) & 0x2FFE0000),
              *(__IO uint32_t *) (BOOTLOADER_ADDRESS + 4), __get_CONTROL());

        osDelay(500);

        __disable_irq();

        ///< 关闭所有使能了中断的外设
        HAL_UART_DeInit(&huart1);
        HAL_UART_DeInit(&huart2);
        HAL_UART_DeInit(&huart3);
        HAL_UART_DeInit(&huart4);
        HAL_UART_DeInit(&huart5);

        HAL_TIM_Base_DeInit(&htim1);
        HAL_TIM_Base_DeInit(&htim2);
        HAL_TIM_Base_DeInit(&htim3);
        HAL_TIM_Base_DeInit(&htim4);
        HAL_TIM_Base_DeInit(&htim7);

        HAL_NVIC_DisableIRQ(EXTI2_IRQn);
        HAL_NVIC_DisableIRQ(EXTI3_IRQn);
        HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

        uint8_t i = 0;
        for (i = 0; i < 82; i++) {
            HAL_NVIC_DisableIRQ((IRQn_Type) i);
            NVIC_ClearPendingIRQ((IRQn_Type) i);
        }

        HAL_NVIC_DisableIRQ((IRQn_Type) -1);
        HAL_NVIC_DisableIRQ((IRQn_Type) -2);
        HAL_NVIC_DisableIRQ((IRQn_Type) -4);
        HAL_NVIC_DisableIRQ((IRQn_Type) -5);
        HAL_NVIC_DisableIRQ((IRQn_Type) -10);
        HAL_NVIC_DisableIRQ((IRQn_Type) -11);
        HAL_NVIC_DisableIRQ((IRQn_Type) -12);
        HAL_NVIC_DisableIRQ((IRQn_Type) -14);

        NVIC_ClearPendingIRQ((IRQn_Type) -1);
        NVIC_ClearPendingIRQ((IRQn_Type) -2);
        NVIC_ClearPendingIRQ((IRQn_Type) -4);
        NVIC_ClearPendingIRQ((IRQn_Type) -5);
        NVIC_ClearPendingIRQ((IRQn_Type) -10);
        NVIC_ClearPendingIRQ((IRQn_Type) -11);
        NVIC_ClearPendingIRQ((IRQn_Type) -12);
        NVIC_ClearPendingIRQ((IRQn_Type) -14);

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) BOOTLOADER_ADDRESS);
        SCB->VTOR = FLASH_BASE;
        ///< 特权模式&使用MSP
        __set_CONTROL(0U);

        JumpToApplication();
    }

    else {
        log_e("Jump target stack pointer error: 0X%lX.", ((*(__IO uint32_t *) BOOTLOADER_ADDRESS) & 0x2FFE0000));
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), jump_boot_update, update_jump_boot, update_jump_boot);

#ifdef __cplusplus
}
#endif

/* @} FAL_UPDATE */
/* @} Robot_FAL */
