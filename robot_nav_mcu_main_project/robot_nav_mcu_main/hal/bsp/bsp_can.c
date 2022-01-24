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
#include "bsp_can.h"
#include "bsp_motor.h"
#include "bsp_battery.h"
#include "can.h"

/**
 * @addtogroup HAL_BSP
 * @{
 */

/**
 * @defgroup BSP_CAN  - CAN
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
extern void motor_data_handle(CAN_HandleTypeDef *hcan);
extern void battery_data_handle(CAN_HandleTypeDef *hcan);

/*****************************************************************/
/**
 * Function:       bsp_can_init
 * Description:    初始化 bsp_can
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
int bsp_can_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_can_deInit
 * Description:    释放 bsp_can 资源
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
int bsp_can_deInit(void) {
    return 0;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    //    CAN_MSG_P mptr =NULL;
    //
    //    mptr = osMailCAlloc(canRxMail, 0);       // Allocate memory
    //
    //    if(mptr == NULL)
    //        return;
    //
    //    /* Get RX message */
    //    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &mptr->header,
    //    mptr->data)
    //    == HAL_OK)
    //    {
    //        osMailPut(canRxMail, mptr);          // Send Mail
    //    }
    //    else
    //    {
    //        /* Reception Error */
    //        Error_Handler();
    //    }
    if (hcan->Instance == CAN_MOTOR_I) {
        motor_data_handle(hcan);
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance == CAN_BATTERY_I) {
        battery_data_handle(hcan);
    }
}

#ifdef __cplusplus
}
#endif

/* @} BSP_CAN */
/* @} Robot_BSP */
