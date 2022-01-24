/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-12
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_ir_nec.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "tim.h"
#include "gpio.h"

#define LOG_TAG "bsp_ir_nec"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_IR_NEC - 红外通信 （参考 NEC 协议）
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
#define AGC_HIGH     5500
#define AGC_BOUNDARY 10000
#define BIT_BOUNDARY 1000

#define AGC_INDEX 0
#define BIT_INDEX 2

#define AGC_EDGES  2
#define BIT_EDGES  16
#define STOP_EDGES 1

#define TX_AGC_HIGH_US   5500
#define TX_AGC_LOW_US    4500
#define TX_BIT01_HIGH_US 500
#define TX_BIT0_LOW_US   500
#define TX_BIT1_LOW_US   1500
#define TX_STOP_HIGH_US  500
#define TX_STOP_LOW_US   3000

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
NEC_HandleTypeDef nec1;
uint8_t           nec1_rx_data;

NEC_HandleTypeDef nec2;
uint8_t           nec2_rx_data;

NEC_HandleTypeDef nec3;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明
 ******************************************************************/
extern uint8_t fal_charge_pull_in(void);
extern void    fal_charge_location_update_mark(uint8_t ir_rx_index, uint8_t ir_tx_index);

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
uint8_t NEC_Receive(NEC_HandleTypeDef *nec, uint8_t *cmd, uint32_t timeout);
uint8_t NEC_Receive_IT(NEC_HandleTypeDef *nec, uint8_t *cmd);
uint8_t NEC_Trimamit(NEC_HandleTypeDef *nec, uint8_t cmd, uint8_t check, uint32_t timeout);

void nec_rx_cplt_callback(NEC_HandleTypeDef *nec);
void nec_rx_error_callback(NEC_HandleTypeDef *nec);
void nec_tx_cplt_callback(NEC_HandleTypeDef *nec);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_ir_nec_init
 * Description:    初始化 bsp_ir_nec
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
int bsp_ir_nec_init(void) {
    /*添加模块处理函数*/
    nec1.type               = NEC_NOT_EXTENDED;
    nec1.state              = NEC_INIT;
    nec1.timerHandle        = &htim2;
    nec1.timerChannel       = TIM_CHANNEL_3;
    nec1.timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_3;
    // nec1.rx_port = MX_TIM2_CH3_IR_RX1_GPIO_Port;
    // nec1.rx_pin = MX_TIM2_CH3_IR_RX1_Pin;

    nec2.type               = NEC_NOT_EXTENDED;
    nec2.state              = NEC_INIT;
    nec2.timerHandle        = &htim2;
    nec2.timerChannel       = TIM_CHANNEL_4;
    nec2.timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_4;
    // nec2.rx_port = MX_TIM2_CH4_IR_RX2_GPIO_Port;
    // nec2.rx_pin =   MX_TIM2_CH4_IR_RX2_Pin;

    nec3.tx_state            = NEC_TX_INIT;
    nec3.tx_pwm_timerHandle  = &htim1;
    nec3.tx_code_timerHandle = &htim3;
    nec3.tx_timerChannel     = TIM_CHANNEL_1;

    /*
    NEC_Receive_IT(&nec2, &nec2_rx_data);
    HAL_Delay(100);
    NEC_Receive_IT(&nec1, &nec1_rx_data);
    */

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_ir_nec_deInit
 * Description:    释放 bsp_ir_nec 资源
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
int bsp_ir_nec_deInit(void) {
    return 0;
}

uint8_t NEC_Receive(NEC_HandleTypeDef *nec, uint8_t *cmd, uint32_t timeout) {
    uint32_t ts;

    if (nec->state != NEC_INIT) {
        return 1;
    }

    ts = HAL_GetTick();

    nec->cmd_p         = cmd;
    nec->is_rx_it_mode = 0;

    bsp_nec_rx_handle(nec);

    while (1) {
        if (nec->state == NEC_OK) {
            nec->state = NEC_INIT;
            return 0;
        }

        if (HAL_GetTick() > ts + timeout) {
            nec->state = NEC_INIT;
            return 1;
        }
    }
}

uint8_t NEC_Receive_IT(NEC_HandleTypeDef *nec, uint8_t *cmd) {
    if (nec->state != NEC_INIT) {
        return 1;
    }

    nec->cmd_p         = cmd;
    nec->is_rx_it_mode = 1;

    bsp_nec_rx_handle(nec);

    return 0;
}

uint8_t nec_read(uint8_t channel) {
    if (channel == 1) {
        return NEC_Receive_IT(&nec1, &nec1_rx_data);
    } else if (channel == 2) {
        return NEC_Receive_IT(&nec2, &nec2_rx_data);
    } else {
        return 1;
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nec_read, nec_read, nec_read);

uint8_t NEC_Receive_IT_Abort(NEC_HandleTypeDef *nec) {
    HAL_TIM_IC_Stop_DMA(nec->timerHandle, nec->timerChannel);

    nec->state = NEC_INIT;

    return 0;
}

uint8_t nec_read_abort(uint8_t channel) {
    if (channel == 1) {
        return NEC_Receive_IT_Abort(&nec1);
    } else if (channel == 2) {
        return NEC_Receive_IT_Abort(&nec2);
    } else {
        return 1;
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nec_read_abort, nec_read_abort, nec_read_abort);

uint8_t NEC_Trimamit(NEC_HandleTypeDef *nec, uint8_t cmd, uint8_t check, uint32_t timeout) {
    uint32_t ts;
    uint8_t  i;
    uint8_t  code0, code1;

    if (nec->tx_state != NEC_TX_INIT) {
        return 1;
    }

    ts = HAL_GetTick();

    nec->tx_cmd = cmd;

    code0 = cmd;

    if (check == NEC_CMD_CHECK) {
        code1 = 0xFF - cmd;
    } else if (check == NEC_PER_DYNA_CHECK) {
        code1 = code0;
    } else if (check == NEC_PER_STATIC_CHECK) {
        code1 = 0;

        for (i = 0; i < 8; i++) {
            if (code0 & (0x01 << i)) {
                code1 |= 0x80 >> i;
            }
        }
    }

    ///< LSB MSB 转换
    nec->tx_code[0] = 0;

    for (i = 0; i < 8; i++) {
        if (code0 & (0x01 << i)) {
            nec->tx_code[0] |= 0x80 >> i;
        }
    }

    nec->tx_code[1] = 0;

    for (i = 0; i < 8; i++) {
        if (code1 & (0x01 << i)) {
            nec->tx_code[1] |= 0x80 >> i;
        }
    }

    bsp_nec_tx_handle(nec);

    while (1) {
        if (nec->tx_state == NEC_TX_OK) {
            nec->tx_state = NEC_TX_INIT;
            return 0;
        }

        if (HAL_GetTick() > ts + timeout) {
            HAL_TIM_Base_Stop_IT(nec->tx_code_timerHandle);
            HAL_TIM_PWM_Stop(nec->tx_pwm_timerHandle, nec->tx_timerChannel);

            __HAL_TIM_SET_COUNTER(nec->tx_code_timerHandle, 0);
            __HAL_TIM_CLEAR_IT(nec->tx_pwm_timerHandle, TIM_IT_UPDATE);
            nec->tx_state = NEC_TX_INIT;

            return 1;
        }
    }
}

uint8_t nec_write(uint8_t cmd, uint8_t check) {
    return NEC_Trimamit(&nec3, cmd, check, 1000);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nec_write, nec_write, nec_write);

void nec_rx_cplt_callback(NEC_HandleTypeDef *nec) {
    uint8_t i;

    for (i = 19; i > 0; i--) {
        nec->rawTimerData[i] = nec->rawTimerData[i] - nec->rawTimerData[i - 1];
    }

    if (nec->timerHandle == nec1.timerHandle && nec->timerChannel == nec1.timerChannel) {
        log_d(
            "nec1 rx ok %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d "
            "%06d %06d %06d %06d %06d %06d %06d %06d %06d",
            nec->rawTimerData[0], nec->rawTimerData[1], nec->rawTimerData[2], nec->rawTimerData[3], nec->rawTimerData[4], nec->rawTimerData[5],
            nec->rawTimerData[6], nec->rawTimerData[7], nec->rawTimerData[8], nec->rawTimerData[9], nec->rawTimerData[10],
            nec->rawTimerData[11], nec->rawTimerData[12], nec->rawTimerData[13], nec->rawTimerData[14], nec->rawTimerData[15],
            nec->rawTimerData[16], nec->rawTimerData[17], nec->rawTimerData[18], nec->rawTimerData[19]);

        log_d("nec1 rx receive cmd[0X%02X].", nec->cmd);

        if (nec->cmd <= 3) {
            fal_charge_location_update_mark(1, nec->cmd);
        } else if (nec->cmd == 0X10) {
            fal_charge_pull_in();
        }

        nec->state = NEC_INIT;

        NEC_Receive_IT(&nec1, &nec1_rx_data);
    } else if (nec->timerHandle == nec2.timerHandle && nec->timerChannel == nec2.timerChannel) {
        log_d(
            "nec2 rx ok %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d "
            "%06d %06d %06d %06d %06d %06d %06d %06d %06d",
            nec->rawTimerData[0], nec->rawTimerData[1], nec->rawTimerData[2], nec->rawTimerData[3], nec->rawTimerData[4], nec->rawTimerData[5],
            nec->rawTimerData[6], nec->rawTimerData[7], nec->rawTimerData[8], nec->rawTimerData[9], nec->rawTimerData[10],
            nec->rawTimerData[11], nec->rawTimerData[12], nec->rawTimerData[13], nec->rawTimerData[14], nec->rawTimerData[15],
            nec->rawTimerData[16], nec->rawTimerData[17], nec->rawTimerData[18], nec->rawTimerData[19]);

        log_d("nec2 rx receive cmd[0X%02X].", nec->cmd);

        if (nec->cmd <= 3) {
            fal_charge_location_update_mark(2, nec->cmd);
        } else if (nec->cmd == 0X10) {
            fal_charge_pull_in();
        }

        nec->state = NEC_INIT;

        NEC_Receive_IT(&nec2, &nec2_rx_data);
    }
}

void nec_rx_error_callback(NEC_HandleTypeDef *nec) {
    uint8_t i;

    for (i = 19; i > 0; i--) {
        nec->rawTimerData[i] = nec->rawTimerData[i] - nec->rawTimerData[i - 1];
    }

    if (nec->timerHandle == nec1.timerHandle && nec->timerChannel == nec1.timerChannel) {
        log_d(
            "nec1 rx error %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d "
            "%06d %06d %06d %06d %06d %06d %06d %06d %06d %06d",
            nec->rawTimerData[0], nec->rawTimerData[1], nec->rawTimerData[2], nec->rawTimerData[3], nec->rawTimerData[4], nec->rawTimerData[5],
            nec->rawTimerData[6], nec->rawTimerData[7], nec->rawTimerData[8], nec->rawTimerData[9], nec->rawTimerData[10],
            nec->rawTimerData[11], nec->rawTimerData[12], nec->rawTimerData[13], nec->rawTimerData[14], nec->rawTimerData[15],
            nec->rawTimerData[16], nec->rawTimerData[17], nec->rawTimerData[18], nec->rawTimerData[19]);
        nec->state = NEC_INIT;
        NEC_Receive_IT(&nec1, &nec1_rx_data);

    }

    else if (nec->timerHandle == nec2.timerHandle && nec->timerChannel == nec2.timerChannel) {
        log_d(
            "nec2 rx error %06d %06d %06d %06d %06d %06d %06d %06d %06d %06d "
            "%06d %06d %06d %06d %06d %06d %06d %06d %06d %06d",
            nec->rawTimerData[0], nec->rawTimerData[1], nec->rawTimerData[2], nec->rawTimerData[3], nec->rawTimerData[4], nec->rawTimerData[5],
            nec->rawTimerData[6], nec->rawTimerData[7], nec->rawTimerData[8], nec->rawTimerData[9], nec->rawTimerData[10],
            nec->rawTimerData[11], nec->rawTimerData[12], nec->rawTimerData[13], nec->rawTimerData[14], nec->rawTimerData[15],
            nec->rawTimerData[16], nec->rawTimerData[17], nec->rawTimerData[18], nec->rawTimerData[19]);
        nec->state = NEC_INIT;
        NEC_Receive_IT(&nec2, &nec2_rx_data);
    }
}

void bsp_nec_tx_handle(NEC_HandleTypeDef *nec) {
    HAL_TIM_Base_Stop_IT(nec->tx_code_timerHandle);
    __HAL_TIM_SET_COUNTER(nec->tx_code_timerHandle, 0);
    __HAL_TIM_CLEAR_IT(nec->tx_code_timerHandle, TIM_IT_UPDATE);

    switch (nec->tx_state) {
        case NEC_TX_INIT:

            __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_AGC_HIGH_US);
            HAL_TIM_PWM_Start(nec->tx_pwm_timerHandle, nec->tx_timerChannel);

            nec->tx_state = NEC_TX_AGC;

            HAL_TIM_Base_Start_IT(nec->tx_code_timerHandle);

            break;

        case NEC_TX_AGC:

            __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_AGC_LOW_US);
            HAL_TIM_PWM_Stop(nec->tx_pwm_timerHandle, nec->tx_timerChannel);

            nec->tx_state     = NEC_TX_BIT;
            nec->tx_bit_cnt   = 0;
            nec->tx_bit_lever = 1;

            HAL_TIM_Base_Start_IT(nec->tx_code_timerHandle);

            break;

        case NEC_TX_BIT:

            if (nec->tx_bit_cnt < BIT_EDGES) {
                if (nec->tx_bit_lever == 1) {
                    __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_BIT01_HIGH_US);
                    HAL_TIM_PWM_Start(nec->tx_pwm_timerHandle, nec->tx_timerChannel);
                    nec->tx_bit_lever = 0;

                } else {
                    if (nec->tx_code[nec->tx_bit_cnt / 8] & (0X01 << (nec->tx_bit_cnt % 8))) {
                        __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_BIT1_LOW_US);
                    } else {
                        __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_BIT0_LOW_US);
                    }

                    HAL_TIM_PWM_Stop(nec->tx_pwm_timerHandle, nec->tx_timerChannel);

                    nec->tx_bit_cnt++;

                    nec->tx_bit_lever = 1;
                }

                nec->tx_state = NEC_TX_BIT;
            } else {
                __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_STOP_HIGH_US);
                HAL_TIM_PWM_Start(nec->tx_pwm_timerHandle, nec->tx_timerChannel);
                nec->tx_state = NEC_TX_STOP_A;
            }

            HAL_TIM_Base_Start_IT(nec->tx_code_timerHandle);

            break;

        case NEC_TX_STOP_A:

            __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_STOP_LOW_US);
            HAL_TIM_PWM_Stop(nec->tx_pwm_timerHandle, nec->tx_timerChannel);
            nec->tx_state = NEC_TX_STOP_B;

            HAL_TIM_Base_Start_IT(nec->tx_code_timerHandle);

            break;

        case NEC_TX_STOP_B:

            __HAL_TIM_SET_AUTORELOAD(nec->tx_code_timerHandle, TX_STOP_HIGH_US);
            HAL_TIM_PWM_Start(nec->tx_pwm_timerHandle, nec->tx_timerChannel);
            nec->tx_state = NEC_TX_STOP_C;

            HAL_TIM_Base_Start_IT(nec->tx_code_timerHandle);

            break;

        case NEC_TX_STOP_C:

            HAL_TIM_PWM_Stop(nec->tx_pwm_timerHandle, nec->tx_timerChannel);

            nec->tx_state = NEC_TX_OK;

            break;

        default:
            break;
    }
}

void bsp_nec_rx_handle(NEC_HandleTypeDef *nec) {
    uint32_t agc_space = 0;
    uint8_t  bit_pos;
    uint32_t bit_space;
    uint8_t  i;

    uint8_t cmd_check;

    HAL_TIM_IC_Stop_DMA(nec->timerHandle, nec->timerChannel);

    switch (nec->state) {
        case NEC_INIT:

            memset(nec->rawTimerData, 0, sizeof(nec->rawTimerData));
            memset(nec->decoded, 0, sizeof(nec->decoded));

            //__HAL_TIM_SET_COUNTER(nec->timerHandle, 0);
            __HAL_TIM_SET_CAPTUREPOLARITY(nec->timerHandle, nec->timerChannel, TIM_INPUTCHANNELPOLARITY_FALLING);

            if (HAL_TIM_IC_Start_DMA(nec->timerHandle, nec->timerChannel, (uint32_t *) &nec->rawTimerData[0], 1) != HAL_OK) {
                log_e("nec dma start error!");
            }

            nec->state = NEC_AGC_START_1;

            break;

        case NEC_AGC_START_1:

            if (HAL_GPIO_ReadPin(nec->rx_port, nec->rx_pin) == GPIO_PIN_RESET) {
                __HAL_TIM_SET_CAPTUREPOLARITY(nec->timerHandle, nec->timerChannel, TIM_INPUTCHANNELPOLARITY_RISING);

                if (HAL_TIM_IC_Start_DMA(nec->timerHandle, nec->timerChannel, (uint32_t *) &nec->rawTimerData[1], 1) != HAL_OK) {
                    log_e("nec dma start error!");
                }

                nec->state = NEC_AGC_START_2;
            } else {
                nec->state = NEC_AGC_FAIL;
                nec_rx_error_callback(nec);
            }

            break;

        case NEC_AGC_START_2:

            if (HAL_GPIO_ReadPin(nec->rx_port, nec->rx_pin) == GPIO_PIN_SET) {
                agc_space = nec->rawTimerData[1] - nec->rawTimerData[0];

                if (agc_space > AGC_HIGH - 250 && agc_space < AGC_BOUNDARY + 250) {
                    nec->state = NEC_AGC_OK;

                    __HAL_TIM_SET_CAPTUREPOLARITY(nec->timerHandle, nec->timerChannel, TIM_INPUTCHANNELPOLARITY_FALLING);
                    HAL_TIM_IC_Start_DMA(nec->timerHandle, nec->timerChannel, (uint32_t *) &nec->rawTimerData[BIT_INDEX],
                                         BIT_EDGES + STOP_EDGES + 1);
                } else {
                    nec->state = NEC_AGC_FAIL;
                    nec_rx_error_callback(nec);
                }
            } else {
                nec->state = NEC_AGC_FAIL;
                nec_rx_error_callback(nec);
            }

            break;

        case NEC_AGC_OK:

            for (bit_pos = 0; bit_pos < BIT_EDGES; bit_pos++) {
                bit_space = nec->rawTimerData[bit_pos + AGC_EDGES + 1] - nec->rawTimerData[bit_pos + AGC_EDGES + 1 - 1];

                if (bit_space > BIT_BOUNDARY + 500) {
                    nec->decoded[bit_pos / 8] |= 1 << (bit_pos % 8);
                } else {
                    nec->decoded[bit_pos / 8] &= ~(1 << (bit_pos % 8));
                }
            }

            cmd_check = ~(nec->decoded[1]);

            if (nec->decoded[0] == cmd_check) {
                ///< MSB 转 LSB
                nec->cmd = 0;

                for (i = 0; i < 8; i++) {
                    if (nec->decoded[0] & (0x01 << i)) {
                        nec->cmd |= 0x80 >> i;
                    }
                }

                if (nec->cmd_p != NULL) {
                    *nec->cmd_p = nec->cmd;
                }

                nec->state = NEC_OK;

                if (nec->is_rx_it_mode) {
                    nec_rx_cplt_callback(nec);
                }
            } else {
                log_e("nec decode0[0X%02X] decode1[0X%02X]", nec->decoded[0], nec->decoded[1]);
                nec->state = NEC_FAIL;

                if (nec->is_rx_it_mode) {
                    nec_rx_error_callback(nec);
                }
            }

            break;

        default:
            break;
    }
}

/*
typedef struct
{
    uint16_t agc[20];
    uint16_t data[2*8+4*8];
    uint16_t stop[8];

}IR_NEC_PULSE_T, *IR_NEC_PULSE_P;

    frame.agc[0] = 1473;
    frame.agc[1] = 1473;
    frame.agc[2] = 1473;
    frame.agc[3] = 1473;
    frame.agc[4] = 1473;
    frame.agc[5] = 1473;
    frame.agc[6] = 1473;
    frame.agc[7] = 1473;
    frame.agc[8] = 1473;
    frame.agc[9] = 1473;
    frame.agc[10] = 1473;
    frame.agc[11] = 0;
    frame.agc[12] = 0;
    frame.agc[13] = 0;
    frame.agc[14] = 0;
    frame.agc[15] = 0;
    frame.agc[16] = 0;
    frame.agc[17] = 0;
    frame.agc[18] = 0;
    frame.agc[19] = 0;

    for(i = 0; ; )
    {
        frame.data[i++] = 1473;
        frame.data[i++] = 0;

        if(i == 16)
            break;
    }

    for(i = 16; ; )
    {
        frame.data[i++] = 1474;
        frame.data[i++] = 0;
        frame.data[i++] = 0;
        frame.data[i++] = 0;

        if(i == 48)
            break;
    }

    frame.stop[0] = 1473;
    frame.stop[1] = 0;
    frame.stop[2] = 0;
    frame.stop[3] = 0;
    frame.stop[4] = 0;
    frame.stop[5] = 0;
    frame.stop[6] = 0;
    frame.stop[7] = 1473;

*/

#ifdef __cplusplus
}
#endif

/* @} BSP_IR_NEC */
/* @} Robot_BSP */
