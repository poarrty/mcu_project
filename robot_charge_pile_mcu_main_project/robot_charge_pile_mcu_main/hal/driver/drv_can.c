/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_can.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 15:49:32
 * @Description: CAN驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_can.h"
#include "task_battery.h"
#include "log.h"

/******************************************************************************
 * @Function: drv_can_init
 * @Description: CAN初始化
 * @Input: void
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 *******************************************************************************/
HAL_StatusTypeDef drv_can_init(void) {
    CAN_FilterTypeDef sFilterConfig;
    HAL_StatusTypeDef status;

    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.SlaveStartFilterBank = 0;
    sFilterConfig.FilterActivation = ENABLE;

    status = HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_CAN_Start(&hcan);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}

/******************************************************************************
 * @Function: drv_can_send_msg
 * @Description: CAN发送
 * @Input: can_tx_msg：发送信息结构体
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 * @param {can_msg_stu_t} can_tx_msg
 *******************************************************************************/
HAL_StatusTypeDef drv_can_send_msg(can_msg_stu_t can_tx_msg) {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    HAL_StatusTypeDef status;

    TxHeader.StdId = can_tx_msg.id;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = can_tx_msg.type;
    TxHeader.DLC = can_tx_msg.data_len;
    TxHeader.TransmitGlobalTime = DISABLE;

    LOG_DEBUG_ARRAY(can_tx_msg.data, can_tx_msg.data_len);

    status =
        HAL_CAN_AddTxMessage(&hcan, &TxHeader, can_tx_msg.data, &TxMailbox);

    if (status != HAL_OK) {
        LOG_ERROR("Return val:%d", status);
        return status;
    }

    return status;
}
