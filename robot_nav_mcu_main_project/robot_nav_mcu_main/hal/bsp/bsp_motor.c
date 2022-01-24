/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-26
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
#include "bsp_motor.h"
#include "can.h"
#include "shell.h"
#include "sys_exc.h"
#include "sys_paras.h"
#include "cmsis_os.h"

#define LOG_TAG "bsp_motor"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_MOTOR - MOTOR
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
 * Function:       bsp_motor_init
 * Description:    初始化 电机控制
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
int bsp_motor_init(void) {
    static uint8_t can_init_flag = 0;
    if (can_init_flag == 0) {
        /*添加模块处理函数*/
        CAN_FilterTypeDef sFilterConfig;
        sFilterConfig.FilterBank           = 0;
        sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh         = 0x0000;
        sFilterConfig.FilterIdLow          = 0x0000;
        sFilterConfig.FilterMaskIdHigh     = 0x0000;
        sFilterConfig.FilterMaskIdLow      = 0x0000;
        sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
        sFilterConfig.FilterActivation     = ENABLE;
        sFilterConfig.SlaveStartFilterBank = 14;

        if (HAL_CAN_ConfigFilter(&CAN_MOTOR_H, &sFilterConfig) == HAL_OK) {
            log_d("can filter success!\r\n");
        } else {
            log_e("can filter error[0X%X]!\r\n", CAN_MOTOR_H.ErrorCode);
            /* Filter configuration Error */
            Error_Handler();
        }

        if (HAL_CAN_Start(&CAN_MOTOR_H) == HAL_OK) {
            log_d("can start success!\r\n");
            HAL_CAN_ActivateNotification(&CAN_MOTOR_H, CAN_IT_RX_FIFO0_MSG_PENDING);
        } else {
            log_e("can start error!\r\n");
            /* Start Error */
            Error_Handler();
        }

#ifdef BINGO
        for (uint8_t i = 0; i < 80; i++) {
            bsp_motor_set_speed(2, 2, 1.0 * sys_paras.motor_reduction_ratio / 10, NO_LOCK);
            osDelay(20);
        }
        can_init_flag++;
#endif
    }

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_motor_deInit
 * Description:    释放 bsp_motor 资源
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
int bsp_motor_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_motor_set_speed
 * Description:    设置电机速度
 * Calls:
 * Called By:
 * @param[in]      speed_l: uint rpm
 * （非轮速，是电机内部转速，到轮子要有减速比）
 * @param[in]      speed_r: unit rpm
 * （非轮速，是电机内部转速，到轮子要有减速比）
 * @param[in]      reduction: 减速比
 * @param[out]     NONE
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
uint8_t bsp_motor_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock) {
    int16_t rpm_min, rpm_max;

    CAN_TxHeaderTypeDef TxHeader;
    uint8_t             TxData[8] = {0x00};
    uint32_t            TxMailbox;

    static bool is_lock = false;

    // CAN_RxHeaderTypeDef RxHeader;
    // uint8_t RxData[8];

    TxHeader.StdId              = 0;
    TxHeader.ExtId              = CAN_ID_SET_DPEED;
    TxHeader.RTR                = CAN_RTR_DATA;
    TxHeader.IDE                = CAN_ID_EXT;
    TxHeader.DLC                = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    rpm_min = -1 * (int32_t) sys_paras.motor_rpm_max;
    rpm_max = 1 * sys_paras.motor_rpm_max;

    speed_l = speed_l * reduction * 10;  ///< 下发值单位为 0.1 rpm, 所以乘以10
    speed_r = speed_r * reduction * 10;  ///< 下发值单位为 0.1 rpm, 所以乘以10

    ///< 底层限速
    speed_l = (speed_l > rpm_max) ? (rpm_max) : (speed_l);
    speed_l = (speed_l < rpm_min) ? (rpm_min) : (speed_l);

    speed_r = (speed_r > rpm_max) ? (rpm_max) : (speed_r);
    speed_r = (speed_r < rpm_min) ? (rpm_min) : (speed_r);

    log_d("Send motor rpm l[%f] r[%f]", speed_l, speed_r);

    ///< 速度值设置
    /*
    TxData[0] = speed_l&0XFF;
    TxData[1] = (speed_l>>8)&0XFF;
    TxData[2] = speed_r&0XFF;
    TxData[3] = (speed_r>>8)&0XFF;
    */

    ///< 控制设置
    ///<
    TxData[4] |= 0X01 << 0;  ///< 使能
    TxData[4] |= 0X01 << 1;  ///< 清除故障

    ///< 紧急停止：不管下发值和实际值，刹车马上闭合
    if (lock == EMERGER_LOCK) {
        speed_l = 0;
        speed_r = 0;
        TxData[4] |= 0X01 << 3;  ///< 启动紧急刹车
    } else {
        TxData[4] &= ~(0X01 << 3);  ///< 关闭紧急刹车
    }

    ///< 正常停止: 下发值需要是0，才能闭合刹车
    ///< 驱动器没有这个功能，不要控制
    /*
    if(lock == NORMAL_LOCK)
    {
        speed_l = 0;
        speed_r = 0;
        TxData[4] |= 0X01<<2;          ///< 启动普通刹车
    }
    else
    {
        TxData[4] &= ~(0X01<<2);       ///< 关闭普通刹车
    }
    */

    // if(lock == NORMAL_LOCK || lock == EMERGER_LOCK || (speed_l==0 &&
    // speed_r==0))            ///< 速度为零，马上刹车
    if (lock == NORMAL_LOCK || lock == EMERGER_LOCK) {
        speed_l = 0;
        speed_r = 0;
        TxData[4] &= ~(0X01 << 4);  ///< 刹车闭合

        is_lock = true;
    } else {
        if (is_lock) {
            speed_l = 0;
            speed_r = 0;

            is_lock = false;
        }

        TxData[4] |= 0X01 << 4;  ///< 刹车释放
    }

    ///< 速度值设置
    int16_t rmpL = speed_l;
    int16_t rmpR = speed_r;
    log_d("----- rmpL[%d] rmpR[%d]", rmpL, rmpR);

    TxData[0] = rmpL & 0XFF;
    TxData[1] = (rmpL >> 8) & 0XFF;
    TxData[2] = rmpR & 0XFF;
    TxData[3] = (rmpR >> 8) & 0XFF;

    if (HAL_CAN_AddTxMessage(&CAN_MOTOR_H, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
        // log_d("Motor speed send success.");
    } else {
        // log_e("Motor speed send error.");
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR */
/* @} Robot_BSP */
