/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-19
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
#include "bsp_battery.h"
#include "can.h"
#include "string.h"
#include "shell.h"
#include "crc16.h"
#include "cmsis_os.h"

#define LOG_TAG "bsp_battery"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_BATTERY  - BATTERY
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
#define BATTERY_BMS_ASK_ID  0X0000052D
#define BATTERY_BMS_RESP_ID 0X00000080

#define BMS_PACK_START 0X80
#define BMS_PACK_END   0X40

#define BMS_BUFF_SIZE 256

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
BATTERY_BMS_DATA_T battery_bms_temp = {0};
BATTERY_BMS_DATA_T battery_bms      = {0};
static uint8_t     bms_buff[BMS_BUFF_SIZE];

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void bms_big2little(BATTERY_BMS_DATA_P bms);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_battery_init
 * Description:    初始化 bsp_battery
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
int bsp_battery_init(void) {
    /*添加模块处理函数*/
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank           = 14;  ///< 这里不能用0
    sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh         = 0x0000;
    sFilterConfig.FilterIdLow          = 0x0000;
    sFilterConfig.FilterMaskIdHigh     = 0x0000;
    sFilterConfig.FilterMaskIdLow      = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
    sFilterConfig.FilterActivation     = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    // CAN1是作为主，而CAN2做为从，做为从的CAN2不能直接访问这个专用的512bytes的SRAM
    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) == HAL_OK) {
        // log_d("battery can filter success!");
    } else {
        // log_d("battery can filter error[0X%X]!", CAN_BATTERY_H.ErrorCode );
        /* Filter configuration Error */
        // Error_Handler();
    }

    if (HAL_CAN_Start(&CAN_BATTERY_H) == HAL_OK) {
        // log_d("battery can start success!");
        // HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    } else {
        // log_d("battery can start error!");
        /* Start Error */
        // Error_Handler();
    }

    memset(&battery_bms, 0, sizeof(battery_bms));
    // battery_bms.cap_full_in = 50000;
    // battery_bms.cap_remain = 50000;

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_deInit
 * Description:    释放 bsp_battery 资源
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
int bsp_battery_deInit(void) {
    return 0;
}

int bsp_battery_current_get(void) {
    return battery_bms.current;
}

uint32_t bsp_battery_volatage_get(void) {
    return battery_bms.voltage;
}

uint32_t bsp_battery_volatage_percent_get(void) {
    uint32_t percent;

    percent = 1.0 * battery_bms.cap_remain / battery_bms.cap_full_in * 100 + 0.5;

    return percent;
}

uint32_t bsp_battery_charge_sta_get(void) {
    return battery_bms.sta_bms.charge_sta;
}

static uint8_t  battery_data_index          = 0;
static uint32_t battery_data_len            = 0;
static bool     is_battery_data_handle_fini = false;

int bsp_battery_bms_update_req(void) {
    CAN_TxHeaderTypeDef TxHeader;

    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    HAL_CAN_DeactivateNotification(&CAN_BATTERY_H, CAN_IT_RX_FIFO1_MSG_PENDING);

    ///< 先清除之前可能没读取的缓存
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO1, &RxHeader, RxData);
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO1, &RxHeader, RxData);
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO1, &RxHeader, RxData);

    battery_data_index          = 0;
    battery_data_len            = 0;
    is_battery_data_handle_fini = false;

    memset(bms_buff, 0, BMS_BUFF_SIZE);

    ///< 发送数据请求
    TxHeader.StdId              = BATTERY_BMS_ASK_ID;
    TxHeader.ExtId              = 0;
    TxHeader.RTR                = CAN_RTR_DATA;
    TxHeader.IDE                = CAN_ID_STD;
    TxHeader.DLC                = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    uint8_t  TxData[8] = {0x00};
    uint32_t TxMailbox;

    uint8_t index = 0;
    uint8_t addr  = 0x10;
    // uint8_t len = sizeof(BATTERY_BMS_DATA_T);
    uint8_t  len = 32;
    uint16_t crc_test;

    TxData[0] = BMS_PACK_START | index;
    TxData[1] = 6;
    TxData[2] = 6;
    TxData[3] = 5;
    TxData[4] = 0;
    TxData[5] = addr;
    TxData[6] = len;
    crc_test  = calculate_crc16(&TxData[1], 6);
    TxData[7] = (uint8_t)(crc_test);

    /* Start the Transmission process */
    HAL_CAN_AddTxMessage(&CAN_BATTERY_H, &TxHeader, TxData, &TxMailbox);

    index++;
    memset(TxData, 0, 8);
    TxData[0]    = BMS_PACK_END | index;
    TxData[1]    = (uint8_t)(crc_test >> 8);
    TxHeader.DLC = 8;
    HAL_CAN_AddTxMessage(&CAN_BATTERY_H, &TxHeader, TxData, &TxMailbox);

    HAL_CAN_ActivateNotification(&CAN_BATTERY_H, CAN_IT_RX_FIFO1_MSG_PENDING);

    return 0;
}

//< battery 数据处理
void battery_data_handle(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    if ((HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) && (RxHeader.StdId == BATTERY_BMS_RESP_ID)) {
        //确保没有丢帧
        if (RxData[0] == battery_data_index) {
            battery_data_index++;
        } else {
            log_e("Battery BMS lose frame!");
            is_battery_data_handle_fini = true;
        }

        if (battery_data_len + RxHeader.DLC - 1 > BMS_BUFF_SIZE) {
            is_battery_data_handle_fini = true;
        }

        else {
            memcpy(bms_buff + battery_data_len, &RxData[1], RxHeader.DLC - 1);
            battery_data_len += RxHeader.DLC - 1;
        }

        //最后一帧数据 并且 收齐77个字节的数据
        if (RxData[0] == 0X0A && battery_data_len == 77) {
            is_battery_data_handle_fini = true;
        }
    }

    if (is_battery_data_handle_fini) {
        HAL_CAN_DeactivateNotification(&CAN_BATTERY_H, CAN_IT_RX_FIFO1_MSG_PENDING);
    }
}

///< update time: min 4ms; max 20ms; timeout recommend: 100ms
int bsp_battery_bms_update_it(uint32_t timeout_ms) {
    int      ret          = -1;
    uint32_t timeout_init = timeout_ms;

    bsp_battery_bms_update_req();

    while (timeout_ms) {
        if (is_battery_data_handle_fini) {
            BATTERY_BMS_FRAME_P p;

            p = (BATTERY_BMS_FRAME_P) bms_buff;

            uint16_t crc_test = calculate_crc16(bms_buff, battery_data_len - 2);

            if (p->crc == crc_test) {
                battery_bms_temp = p->bms_data;

                //大端转换小端
                bms_big2little(&battery_bms_temp);

                battery_bms = battery_bms_temp;

                log_d(
                    "Battery BMS update it mode ok, per %d charge %d, take %d "
                    "ms",
                    bsp_battery_volatage_percent_get(), bsp_battery_charge_sta_get(), timeout_init - timeout_ms);
                log_d("BMS detail: ..............");
                log_d("current %d ", battery_bms.current);
                log_d("cap_full_in %d ", battery_bms.cap_full_in);
                log_d("cap_full_out %d ", battery_bms.cap_full_out);
                log_d("cap_remain %d ", battery_bms.cap_remain);
                log_d("voltage %d ", battery_bms.voltage);
                log_d("rsos %d ", battery_bms.rsos);
                log_d("cyc_time %d ", battery_bms.cyc_time);
                log_d("sta_protect 0x%x ", battery_bms.sta_protect);
                log_d("sta_warming 0x%x ", battery_bms.sta_warming);
                log_d("sta_bms 0x%x ", battery_bms.sta_bms);
                log_d("sta_bla 0x%x ", battery_bms.sta_bla);
                log_d("..........................");

                ret = 0;

                break;

            } else {
                log_e("bms update it mode error.");

                ret = -1;

                break;
            }
        }

        if (osKernelGetState() == osKernelRunning) {
            osDelay(1);
        } else {
            HAL_Delay(1);
        }

        timeout_ms--;
    }

    return ret;
}

///< update time: min 4ms; max 20ms; timeout recommend: 100ms
int bsp_battery_bms_update(uint32_t timeout_ms) {
    int                 ret = -1;
    CAN_TxHeaderTypeDef TxHeader;

    CAN_RxHeaderTypeDef RxHeader;
    uint8_t             RxData[8];

    ///< 先清除之前可能没读取的缓存
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO0, &RxHeader, RxData);
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO0, &RxHeader, RxData);
    HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO0, &RxHeader, RxData);

    memset(bms_buff, 0, BMS_BUFF_SIZE);

    ///< 发送数据请求
    TxHeader.StdId              = BATTERY_BMS_ASK_ID;
    TxHeader.ExtId              = 0;
    TxHeader.RTR                = CAN_RTR_DATA;
    TxHeader.IDE                = CAN_ID_STD;
    TxHeader.DLC                = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    uint8_t  TxData[8] = {0x00};
    uint32_t TxMailbox;

    uint8_t index = 0;
    uint8_t addr  = 0x10;
    // uint8_t len = sizeof(BATTERY_BMS_DATA_T);
    uint8_t  len = 32;
    uint16_t crc_test;

    TxData[0] = BMS_PACK_START | index;
    TxData[1] = 6;
    TxData[2] = 6;
    TxData[3] = 5;
    TxData[4] = 0;
    TxData[5] = addr;
    TxData[6] = len;
    crc_test  = calculate_crc16(&TxData[1], 6);
    TxData[7] = (uint8_t)(crc_test);
    // TxData[8] = (uint8_t)(crc_test >> 8);

    /* Start the Transmission process */
    if (HAL_CAN_AddTxMessage(&CAN_BATTERY_H, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        // log_d("battery CAN TX error!");
        /* Transmission request Error */
        // Error_Handler();
    } else {
        // log_d("battery CAN TX success");
    }

    // HAL_Delay(10);

    index++;
    memset(TxData, 0, 8);
    TxData[0]    = BMS_PACK_END | index;
    TxData[1]    = (uint8_t)(crc_test >> 8);
    TxHeader.DLC = 8;
    HAL_CAN_AddTxMessage(&CAN_BATTERY_H, &TxHeader, TxData, &TxMailbox);

    ///< 接收电池 BMS 数据
    uint32_t ts_start = HAL_GetTick();
    uint32_t ts_end;
    uint8_t  rx_len = 0;

    uint8_t rx_index = 0;

    while (1) {
        if (HAL_CAN_GetRxFifoFillLevel(&CAN_BATTERY_H, CAN_RX_FIFO1) != 0) {
            // log_d("Battery BMS get pack");

            if ((HAL_CAN_GetRxMessage(&CAN_BATTERY_H, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) && (RxHeader.StdId == BATTERY_BMS_RESP_ID)) {
                // log_d("B %d", RxData[0]&0x1f);

                //确保没有丢帧
                if (RxData[0] == rx_index) {
                    rx_index++;
                } else {
                    log_e("Battery BMS lose frame!");
                    ret = -1;
                    break;
                }

                if (rx_len + RxHeader.DLC - 1 > BMS_BUFF_SIZE) {
                    ret = -1;
                    break;
                }

                else {
                    memcpy(bms_buff + rx_len, &RxData[1], RxHeader.DLC - 1);
                    rx_len += RxHeader.DLC - 1;
                }

                //最后一帧数据 并且 收齐77个字节的数据
                if (RxData[0] == 0X0A && rx_len == 77) {
                    ts_end = HAL_GetTick();

                    BATTERY_BMS_FRAME_P p;

                    p = (BATTERY_BMS_FRAME_P) bms_buff;

                    uint16_t crc_test = calculate_crc16(bms_buff, rx_len - 2);

                    if (p->crc == crc_test)
                    // if(1)
                    {
                        battery_bms_temp = p->bms_data;

                        //大端转换小端
                        bms_big2little(&battery_bms_temp);

                        battery_bms = battery_bms_temp;

                        ret = 0;

                        log_d(
                            "Battery BMS update ok, per %d charge %d, take %d "
                            "ms",
                            bsp_battery_volatage_percent_get(), bsp_battery_charge_sta_get(), ts_end - ts_start);
                        log_d("BMS detail: ..............");
                        log_d("current %d ", battery_bms.current);
                        log_d("cap_full_in %d ", battery_bms.cap_full_in);
                        log_d("cap_full_out %d ", battery_bms.cap_full_out);
                        log_d("cap_remain %d ", battery_bms.cap_remain);
                        log_d("voltage %d ", battery_bms.voltage);
                        log_d("rsos %d ", battery_bms.rsos);
                        log_d("cyc_time %d ", battery_bms.cyc_time);
                        log_d("sta_protect 0x%x ", battery_bms.sta_protect);
                        log_d("sta_warming 0x%x ", battery_bms.sta_warming);
                        log_d("sta_bms 0x%x ", battery_bms.sta_bms);
                        log_d("sta_bla 0x%x ", battery_bms.sta_bla);
                        log_d("..........................");

                    } else {
                        ret = -1;
                        log_e(
                            "Battery BMS update error[%d] CAL CRC[%#02X] "
                            "WANT[%#02X]",
                            rx_len, p->crc, crc_test);
                    }

                    break;
                }
            }
        } else {
            if (HAL_GetTick() > ts_start + timeout_ms) {
                log_e("Battery BMS update timeout! len[%d]", rx_len);
                ret = -1;
                break;
            } else {
                // HAL_Delay(5);
            }
        }
    }

    return ret;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bmsUpdate, bsp_battery_bms_update, bsp_battery_bms_update);

static void data_big2little(uint8_t *data, uint8_t size) {
    uint8_t temp[2];
    uint8_t i;

    for (i = 0; i < size / 2; i++) {
        temp[0] = data[i];
        temp[1] = data[size - i - 1];

        data[i]            = temp[1];
        data[size - i - 1] = temp[0];
    }
}

static void bms_big2little(BATTERY_BMS_DATA_P bms) {
    data_big2little((uint8_t *) &bms->current, sizeof(bms->current));
    data_big2little((uint8_t *) &bms->cap_full_in, sizeof(bms->cap_full_in));
    data_big2little((uint8_t *) &bms->cap_full_out, sizeof(bms->cap_full_out));
    data_big2little((uint8_t *) &bms->cap_remain, sizeof(bms->cap_remain));
    data_big2little((uint8_t *) &bms->voltage, sizeof(bms->voltage));
    data_big2little((uint8_t *) &bms->rsos, sizeof(bms->rsos));
    data_big2little((uint8_t *) &bms->cyc_time, sizeof(bms->cyc_time));

    data_big2little((uint8_t *) &bms->sta_protect, sizeof(bms->sta_protect));
    data_big2little((uint8_t *) &bms->sta_warming, sizeof(bms->sta_warming));
    data_big2little((uint8_t *) &bms->sta_bms, sizeof(bms->sta_bms));
    data_big2little((uint8_t *) &bms->sta_bla, sizeof(bms->sta_bla));

    data_big2little((uint8_t *) &bms->temp_cell1, sizeof(bms->temp_cell1));
    data_big2little((uint8_t *) &bms->temp_cell2, sizeof(bms->temp_cell2));
    data_big2little((uint8_t *) &bms->temp_cell3, sizeof(bms->temp_cell3));
    data_big2little((uint8_t *) &bms->temp_cell4, sizeof(bms->temp_cell4));
    data_big2little((uint8_t *) &bms->temp_env, sizeof(bms->temp_env));
    data_big2little((uint8_t *) &bms->temp_power_board1, sizeof(bms->temp_power_board1));
    data_big2little((uint8_t *) &bms->temp_power_board2, sizeof(bms->temp_power_board2));
    data_big2little((uint8_t *) &bms->vol_cell11, sizeof(bms->vol_cell11));
    data_big2little((uint8_t *) &bms->vol_cell12, sizeof(bms->vol_cell12));
    data_big2little((uint8_t *) &bms->vol_cell13, sizeof(bms->vol_cell13));
    data_big2little((uint8_t *) &bms->vol_cell14, sizeof(bms->vol_cell14));
    data_big2little((uint8_t *) &bms->vol_cell15, sizeof(bms->vol_cell15));
    data_big2little((uint8_t *) &bms->vol_cell16, sizeof(bms->vol_cell16));
    data_big2little((uint8_t *) &bms->vol_cell17, sizeof(bms->vol_cell17));
    data_big2little((uint8_t *) &bms->vol_cell18, sizeof(bms->vol_cell18));
    data_big2little((uint8_t *) &bms->vol_cell19, sizeof(bms->vol_cell19));
    data_big2little((uint8_t *) &bms->vol_cell110, sizeof(bms->vol_cell110));
    data_big2little((uint8_t *) &bms->vol_cell111, sizeof(bms->vol_cell111));
    data_big2little((uint8_t *) &bms->vol_cell112, sizeof(bms->vol_cell112));
    data_big2little((uint8_t *) &bms->vol_cell113, sizeof(bms->vol_cell113));
    data_big2little((uint8_t *) &bms->vol_cell114, sizeof(bms->vol_cell114));
}

#ifdef __cplusplus
}
#endif

/* @} BSP_BATTERY */
/* @} Robot_BSP */
