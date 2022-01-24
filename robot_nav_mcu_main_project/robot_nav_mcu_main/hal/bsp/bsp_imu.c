/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-24
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
#include "bsp_imu.h"
#include "stdint.h"
#include "gpio.h"
#include "usart.h"
#include "string.h"
#include "shell.h"
#include "cmsis_os.h"
#define LOG_TAG "bsp_imu"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_IMU - IMU
 *
 * @brief 惯导模块BSP\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define IMU_INIT_TIMEOUT_MS 10000

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
const static uint8_t IMU_FRAME_ACK[11] = {0X3A, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X00, 0X0D, 0X0A};

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_imu_init
 * Description:    初始化 bsp_imu
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
int bsp_imu_init(void) {
    /*添加模块处理函数*/
    uint32_t ts_start;
    uint8_t  cmd_res;

    ts_start = HAL_GetTick();

    bsp_imu_reset();

    if (osKernelGetState() == osKernelRunning) {
        osDelay(500);
    } else {
        HAL_Delay(500);
    }

    ///< 先清空串口可能的遗留数据
    uint8_t rx_buff[10];
    HAL_UART_Receive(&IMU_USART_H, rx_buff, 10, 100);

    while (1) {
        osKernelLock();
        cmd_res = bsp_imu_set_mode(IMU_MODE_CMD);
        osKernelUnlock();

        if (cmd_res == 0) {
            log_d("IMU set mode [CMD] success.");
            break;
        } else {
            log_e("IMU set mod [CMD] error.");
            // return 1;
        }

        if (osKernelGetState() == osKernelRunning) {
            osDelay(500);
        } else {
            HAL_Delay(500);
        }

        if (HAL_GetTick() > ts_start + IMU_INIT_TIMEOUT_MS) {
            return 1;
        }
    }

    while (1) {
        osKernelLock();
        cmd_res = bsp_imu_set_stream_freq(STREAM_FREQ_100);
        osKernelUnlock();

        if (cmd_res == 0) {
            log_d("IMU set stream freq %d Hz success.", STREAM_FREQ_100);
            break;
        } else {
            log_e("IMU set stream freq %d Hz error.", STREAM_FREQ_100);
            // return 1;
        }

        if (osKernelGetState() == osKernelRunning) {
            osDelay(500);
        } else {
            HAL_Delay(500);
        }

        if (HAL_GetTick() > ts_start + IMU_INIT_TIMEOUT_MS) {
            return 1;
        }
    }

    while (1) {
        osKernelLock();
        cmd_res = bsp_imu_set_degrad_unit(0);
        osKernelUnlock();

        if (cmd_res == 0) {
            log_d("IMU set degrad [%d] success.", 0);
            break;
        } else {
            log_e("IMU set degrad [%d] error.", 0);
            // return 1;
        }

        if (osKernelGetState() == osKernelRunning) {
            osDelay(500);
        } else {
            HAL_Delay(500);
        }

        if (HAL_GetTick() > ts_start + IMU_INIT_TIMEOUT_MS) {
            return 1;
        }
    }

    while (1) {
        osKernelLock();
        cmd_res = bsp_imu_set_mode(IMU_MODE_STREAM);
        osKernelUnlock();

        if (cmd_res == 0) {
            log_d("IMU set mode mode [STREAM] success.");
            break;
        } else {
            log_e("IMU set mode [STREAM] error.");
            // return 1;
        }

        if (osKernelGetState() == osKernelRunning) {
            osDelay(500);
        } else {
            HAL_Delay(500);
        }

        if (HAL_GetTick() > ts_start + IMU_INIT_TIMEOUT_MS) {
            return 1;
        }
    }

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_imu_init, bsp_imu_init, bsp_imu_init);

/*****************************************************************/
/**
 * Function:       bsp_imu_deInit
 * Description:    释放 bsp_imu 资源
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
int bsp_imu_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_imu_reset
 * Description:    IMU 器件复位
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
void bsp_imu_reset(void) {
    HAL_GPIO_WritePin(MX_IMU_RSTN_GPIO_Port, MX_IMU_RSTN_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(MX_IMU_RSTN_GPIO_Port, MX_IMU_RSTN_Pin, GPIO_PIN_SET);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_imu_reset, bsp_imu_reset, bsp_imu_reset);

/*****************************************************************/
/**
 * Function:       bsp_imu_set_mode
 * Description:    设置 IMU 工作模式
 * Calls:
 * Called By:
 * @param[in]      is_stream: [0]命令模式   [1]弧度
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
uint8_t bsp_imu_set_mode(IMU_MODE_E mode) {
    uint8_t ret = 0;
    // uint8_t recover_flag = 0;
    uint8_t cmd_mode_buff[11]    = {0x3A, 0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0D, 0x0A};
    uint8_t stream_mode_buff[11] = {0x3A, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0D, 0x0A};

    uint8_t *tx_buff_p;
    uint8_t  rx_buff[11] = {0x00};
    ;

    ///< 检查之前是否处于中断接收状态
    /*
    if(IMU_USART_H.RxState == HAL_UART_STATE_BUSY_RX)
    {
        HAL_UART_AbortReceive_IT(&IMU_USART_H);
        recover_flag = 1;
    }
    */

    if (mode == IMU_MODE_CMD) {
        tx_buff_p = cmd_mode_buff;
    } else if (mode == IMU_MODE_STREAM) {
        tx_buff_p = stream_mode_buff;
    } else {
        ret = 1;
    }

    if (ret == 0) {
        HAL_UART_Transmit(&IMU_USART_H, tx_buff_p, 11, 100);

        /* 正常情况下，5ms 内返回设置结果 */
        HAL_StatusTypeDef uart_ret = HAL_UART_Receive(&IMU_USART_H, rx_buff, 11, 20);

        if (uart_ret == HAL_OK) {
            if (bsp_imu_frame_is_ack(rx_buff)) {
                ret = 0;
            } else {
                log_e("imu ack check error.");
                elog_hexdump("imu ack data", 11, rx_buff, 11);
                ret = 1;
            }
        } else {
            log_e("wait imu resp error[%d].", uart_ret);
            ret = 1;
        }
    }

    ///< 检查是否需要恢复为中断接收状态
    /*
    if(recover_flag == 1)
    {
        HAL_UART_Receive_IT(&IMU_USART_H, &imu_recv_byte, 1);
    }
    */

    return ret;
}

/*****************************************************************/
/**
 * Function:       bsp_imu_set_stream_freq
 * Description:    设置 IMU 流模式数据频率
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
uint8_t bsp_imu_set_stream_freq(STREAM_FREQ_E freq) {
    uint8_t ret = 0;
    // uint8_t recover_flag = 0;
    IMU_FRAME_SET_T cmd;
    uint8_t         rx_buff[11] = {0x00};

    ///< 检查之前是否处于中断接收状态
    /*
    if(IMU_USART_H.RxState == HAL_UART_STATE_BUSY_RX)
    {
        HAL_UART_AbortReceive_IT(&IMU_USART_H);
        recover_flag = 1;
    }
    */

    cmd.header    = IMU_FRAME_HEADER;
    cmd.sensor_id = IMU_ADDR;
    cmd.cmd       = IMU_SET_STREAM_FREQ;
    cmd.data_len  = 4;
    cmd.data      = freq;
    cmd.lrc       = bsp_imu_frame_check_sum((uint8_t *) &cmd.sensor_id, 10);
    cmd.end1      = IMU_FRAME_END1;
    cmd.end2      = IMU_FRAME_END2;

    HAL_UART_Transmit(&IMU_USART_H, (uint8_t *) &cmd, sizeof(cmd), 100);

    /* 正常情况下，5ms 内返回设置结果 */
    if (HAL_UART_Receive(&IMU_USART_H, rx_buff, 11, 20) == HAL_OK) {
        if (bsp_imu_frame_is_ack(rx_buff)) {
            ret = 0;
        } else {
            ret = 1;
        }
    } else {
        ret = 1;
    }

    ///< 检查是否需要恢复为中断接收状态
    /*
    if(recover_flag == 1)
    {
        HAL_UART_Receive_IT(&IMU_USART_H, &imu_recv_byte, 1);
    }
    */

    return ret;
}

/*****************************************************************/
/**
 * Function:       bsp_imu_set_degrad_unit
 * Description:    设置 角度和角速率单位
 * Calls:
 * Called By:
 * @param[in]      is_radian: [0]度   [1]弧度
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
uint8_t bsp_imu_set_degrad_unit(uint8_t is_radian) {
    uint8_t ret = 0;
    // uint8_t recover_flag = 0;
    IMU_FRAME_SET_T cmd;
    uint8_t         rx_buff[11] = {0x00};

    ///< 检查之前是否处于中断接收状态
    /*
    if(IMU_USART_H.RxState == HAL_UART_STATE_BUSY_RX)
    {
        HAL_UART_AbortReceive_IT(&IMU_USART_H);
        recover_flag = 1;
    }
    */

    cmd.header    = IMU_FRAME_HEADER;
    cmd.sensor_id = IMU_ADDR;
    cmd.cmd       = IMU_SET_DEGRAD_OUTPUT;
    cmd.data_len  = 4;

    if (is_radian) {
        cmd.data = 1;
    } else {
        cmd.data = 0;
    }

    cmd.lrc  = bsp_imu_frame_check_sum((uint8_t *) &cmd.sensor_id, 10);
    cmd.end1 = IMU_FRAME_END1;
    cmd.end2 = IMU_FRAME_END2;

    HAL_UART_Transmit(&IMU_USART_H, (uint8_t *) &cmd, sizeof(cmd), 100);

    /* 正常情况下，5ms 内返回设置结果 */
    if (HAL_UART_Receive(&IMU_USART_H, rx_buff, 11, 20) == HAL_OK) {
        if (bsp_imu_frame_is_ack(rx_buff)) {
            ret = 0;
        } else {
            ret = 1;
        }
    } else {
        ret = 1;
    }

    ///< 检查是否需要恢复为中断接收状态
    /*
    if(recover_flag == 1)
    {
        HAL_UART_Receive_IT(&IMU_USART_H, &imu_recv_byte, 1);
    }
    */

    return ret;
}

/*****************************************************************/
/**
 * Function:       bsp_imu_frame_check_sum
 * Description:    IMU 通信数据校验函数
 * Calls:
 * Called By:
 * @param[in]      is_radian: [0]度   [1]弧度
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
uint16_t bsp_imu_frame_check_sum(uint8_t *data, uint16_t len) {
    uint16_t check_sum = 0;
    uint16_t i;

    for (i = 0; i < len; i++) {
        check_sum += data[i];
    }

    return check_sum;
}

/*****************************************************************/
/**
 * Function:       bsp_imu_frame_is_ack
 * Description:    IMU 应答检查
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
uint8_t bsp_imu_frame_is_ack(uint8_t *frame) {
    if (memcmp(IMU_FRAME_ACK, frame, sizeof(IMU_FRAME_ACK)) == 0) {
        return 1;
    } else {
        return 0;
    }
}

#ifdef __cplusplus
}
#endif

/* @} Robot_BSP */
/* @} Robot_HAL */
