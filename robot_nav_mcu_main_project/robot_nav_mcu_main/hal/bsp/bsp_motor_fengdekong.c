/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         linzhongxing
 ** Version:        V0.0.1
 ** Date:           2021-8-2
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-08 linzhongxing 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-8-2       linzhongxing    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_motor_fengdekong.h"
#include "can.h"
#include "shell.h"
#include "sys_exc.h"
#include "sys_paras.h"
#include "typedef.h"
#include "cmsis_os.h"

#define LOG_TAG "bsp_motor"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_MOTOR_fengdekong -
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
#define s32      int
#define delay_ms HAL_Delay

// canopen区域

#define PP_Mode 1
#define PV_Mode 3
#define PT_Mode 4

// SDO CMD
#define SDO_W1 0x2F
#define SDO_W2 0x2B
#define SDO_W4 0x23
#define SDO_RD 0x40

// COBID
#define COBID_601 0x601

#define Left_Wheel  0x60FF
#define Right_Wheel 0x68FF

// Object dictionary of CANopen

#define Control_word                 0x6040
#define Status_word                  0x6041
#define Modes_of_operation           0x6060
#define Modes_0f_operation_display   0x6061
#define Position_actual_value        0x6063
#define Velocity_sensor_actual_value 0x6069
#define Velocity_actual_value        0x606C
#define Target_torque                0x6071
#define Target_position              0x607A
#define Profile_velocity             0x6081
#define Profile_accleration          0x6083
#define Profile_deceleration         0x6084
#define Torque_slope                 0x6087
#define Position_factor              0x6093
#define Target_velocity              0x60FF

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
static void CANopen_PV_Init(void);
static void Motor_enable(void);
static u8   SDO_Write_OD(u8 CMD, u16 Index, u8 SubIndex, u32 DATA);
static u8   SDO_Write_OD_Ndly(u8 CMD, u16 Index, u8 SubIndex, u32 DATA);

static u8 CAN1_Send(u16 Id, u8 CAN1_DLC);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_motor_fengdekong_init
 * Description:    初始化 电机控制 东兴昌驱动器
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
int bsp_motor_fengdekong_init(void) {
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

        if (HAL_CAN_ConfigFilter(&SYNTRON_CAN_MOTOR_H, &sFilterConfig) == HAL_OK) {
            log_d("can filter success!\r\n");
        } else {
            log_e("can filter error[0X%X]!\r\n", SYNTRON_CAN_MOTOR_H.ErrorCode);
            /* Filter configuration Error */
            Error_Handler();
        }

        if (HAL_CAN_Start(&SYNTRON_CAN_MOTOR_H) == HAL_OK) {
            log_d("can start success!\r\n");
            HAL_CAN_ActivateNotification(&SYNTRON_CAN_MOTOR_H, CAN_IT_RX_FIFO0_MSG_PENDING);
        } else {
            log_e("can start error!\r\n");
            /* Start Error */
            Error_Handler();
        }

        can_init_flag = 1;
    }

    CANopen_PV_Init();
    Motor_enable();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_motor_fengdekong_init, bsp_motor_fengdekong_init,
                 bsp_motor_fengdekong_init);

/*****************************************************************/
/**
 * Function:       bsp_motor_fengdekong_deInit
 * Description:    释放 bsp_motor_fengdekong_deInit 资源
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
int bsp_motor_fengdekong_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_motor_fengdekong_set_speed
 * Description:    设置电机速度
 * Calls:
 * Called By:
 * @param[in]      speed_l: uint 0.1 rpm
 * （非轮速，是电机内部转速，到轮子要有减速比）
 * @param[in]      speed_r: unit 0.1 rpm
 * （非轮速，是电机内部转速，到轮子要有减速比）
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
uint8_t bsp_motor_fengdekong_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock) {
    int16_t rpm_min, rpm_max;

    // return 0;

    ///< 左右轮转向定义相反
    speed_l = speed_l * -1;

    rpm_min = -1 * (int32_t) sys_paras.motor_rpm_max;
    rpm_max = 1 * sys_paras.motor_rpm_max;

    ///< 底层限速
    speed_l = (speed_l > rpm_max) ? (rpm_max) : (speed_l);
    speed_l = (speed_l < rpm_min) ? (rpm_min) : (speed_l);

    speed_r = (speed_r > rpm_max) ? (rpm_max) : (speed_r);
    speed_r = (speed_r < rpm_min) ? (rpm_min) : (speed_r);
#if 0
	speed_l = speed_l * reduction * 10;
	speed_r = speed_r * reduction * 10;
#endif
    log_d("Send motor syntron drv l[%d] r[%d]", (int16_t) speed_l, (int16_t) speed_r);

    SDO_Write_OD_Ndly(SDO_W4, 0x60FF, 0x00, (int16_t) speed_l);

    if (osKernelGetState() == osKernelRunning) {
        osDelay(5);
    } else {
        HAL_Delay(5);
    }

    SDO_Write_OD_Ndly(SDO_W4, 0x68FF, 0x00, (int16_t) speed_r);

    return 0;
}

uint8_t bsp_motor_fengdekong_req_pos(void) {
    SDO_Write_OD_Ndly(SDO_RD, 0x6063, 0x00, 0);
    SDO_Write_OD_Ndly(SDO_RD, 0x6863, 0x00, 0);

    return 0;
}

static u8 CAN1Sedbuf[8];

/*模式设置*/
static u8 Contol_Mode_SET(u8 CANopen_mode) {
    CAN1Sedbuf[0] = SDO_W1;
    CAN1Sedbuf[1] = 0x60;
    CAN1Sedbuf[2] = 0x60;
    CAN1Sedbuf[3] = 0x00;
    CAN1Sedbuf[4] = CANopen_mode;

    CAN1_Send(COBID_601, 5);
    delay_ms(5);
    return (1);
}
/*激活节点*/
static u8 CANopen_Activate(u8 CANopen_ID) {
    CAN1Sedbuf[0] = 0x01;
    CAN1Sedbuf[1] = CANopen_ID;
    CAN1_Send(0x000, 2);
    delay_ms(5);
    return (1);
}

static u8 SDO_Write_OD(u8 CMD, u16 Index, u8 SubIndex, u32 DATA) {
    CAN1Sedbuf[0] = CMD;
    CAN1Sedbuf[1] = (u8)(Index & 0xFF);
    CAN1Sedbuf[2] = (u8)(Index >> 8 & 0xFF);
    CAN1Sedbuf[3] = SubIndex;
    CAN1Sedbuf[4] = (u8)(DATA & 0xFF);
    CAN1Sedbuf[5] = (u8)(DATA >> 8 & 0xFF);
    CAN1Sedbuf[6] = (u8)(DATA >> 16 & 0xFF);
    CAN1Sedbuf[7] = (u8)(DATA >> 24 & 0xFF);
    CAN1_Send(COBID_601, 8);
    delay_ms(5);
    return (1);
}

static u8 SDO_Write_OD_Ndly(u8 CMD, u16 Index, u8 SubIndex, u32 DATA) {
    CAN1Sedbuf[0] = CMD;
    CAN1Sedbuf[1] = (u8)(Index & 0xFF);
    CAN1Sedbuf[2] = (u8)(Index >> 8 & 0xFF);
    CAN1Sedbuf[3] = SubIndex;
    CAN1Sedbuf[4] = (u8)(DATA & 0xFF);
    CAN1Sedbuf[5] = (u8)(DATA >> 8 & 0xFF);
    CAN1Sedbuf[6] = (u8)(DATA >> 16 & 0xFF);
    CAN1Sedbuf[7] = (u8)(DATA >> 24 & 0xFF);
    CAN1_Send(COBID_601, 8);
    // delay_ms(5);
    return (1);
}

static void CANopen_PV_Init(void) {
    // STEP1:激活节点1
    CANopen_Activate(0x01);

    // STEP2:设置速度模式  6060H写为3
    Contol_Mode_SET(PV_Mode);

// STEP3:设置加减速 写6083H和6084H
#if 0
	SDO_Write_OD(  SDO_W4, Profile_accleration | Left_Wheel, 0x00, 200000 - sys_paras.motor_acc_up * 1000);
	SDO_Write_OD(  SDO_W4, Profile_deceleration | Left_Wheel, 0x00, 200000 - sys_paras.motor_acc_down * 1000);


	SDO_Write_OD( SDO_W4, Profile_accleration | Right_Wheel, 0x00, 200000 - sys_paras.motor_acc_up * 1000);
	SDO_Write_OD( SDO_W4, Profile_accleration | Right_Wheel, 0x00, 200000 - sys_paras.motor_acc_down * 1000);
#endif

    // STEP4:设置目标转速为0  写60FFH

    SDO_Write_OD(SDO_W4, Left_Wheel, 0x00, 0x00000000);
    SDO_Write_OD(SDO_W4, Right_Wheel, 0x00, 0x00000000);

    // STEP5:设置位置反馈

    SDO_Write_OD(0x22, 0x1800, 0x01, 0x80000181);  // TPDO0 STOP

    SDO_Write_OD(0x22, 0x1801, 0x01, 0x80000281);    // TPDO1 STOP
    SDO_Write_OD(SDO_W1, 0x1A00, 0x00, 0x00000000);  // TPDO1 CLEAR
    SDO_Write_OD(0x22, 0x1A00, 0x01, 0x60640020);    //左
    SDO_Write_OD(0x22, 0x1A00, 0x02, 0x68640020);    //右
    SDO_Write_OD(SDO_W1, 0x1A00, 0x00,
                 0x00000002);  //设置 TXPDO 1 映射的对象个数为 2

    SDO_Write_OD(SDO_W1, 0x1801, 0x05, 20);   // PERIOD 20 ms
    SDO_Write_OD(SDO_W1, 0x1801, 0x03, 20);   // PERIOD 20 ms
    SDO_Write_OD(SDO_W1, 0x1801, 0x02, 254);  // PERIOD UPLOAD

    SDO_Write_OD(0x22, 0x1801, 0x01, 0x00000281);  // TPDO1 START
}

static void Motor_enable(void) {
    //使能写0x6040分别为6、7、F

    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x06);
    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x07);

    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x0F);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fengdekong_enable, Motor_enable, syntron_enable);

void bsp_fengdekong_motor_disable(void) {
    //失能写0x6040
    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x06);
    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x07);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fengdekong_disable, bsp_fengdekong_motor_disable,
                 bsp_fengdekong_motor_disable);

void bsp_fengdekong_motor_enable(void) {
    SDO_Write_OD(SDO_W2, 0x6040, 0x00, 0x0F);
}

static u8 CAN1_Send(u16 Id, u8 CAN1_DLC) {
    /*
    u8 mail;
    u16 i=0;
    TxMes1.StdId = Id;           // 标准标识符ID
    TxMes1.ExtId = Id;           // 扩展标示符ID（29位）
    TxMes1.IDE = CAN_Id_Standard; // 使用标准贞
    TxMes1.RTR = CAN_RTR_Data;       // 消息类型为数据帧；/遥控帧
    TxMes1.DLC = CAN1_DLC;               //     1·报文长度
    for(i=0;i<CAN1_DLC;i++)
    TxMes1.Data[i]=CAN1Sedbuf[i];              // 第一帧信息
     mail = CAN_Transmit(CAN1, &TxMes1);
     i=0;
     while((CAN_TransmitStatus(CAN1, mail)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;
    //等待发送结束 if(i>=0XFFF)return 1; return 0;
    */
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t             TxData[8] = {0x00};
    uint32_t            TxMailbox;
    uint8_t             i;

    TxHeader.StdId              = Id;
    TxHeader.ExtId              = Id;
    TxHeader.RTR                = CAN_RTR_DATA;
    TxHeader.IDE                = CAN_ID_STD;
    TxHeader.DLC                = CAN1_DLC;
    TxHeader.TransmitGlobalTime = DISABLE;

    for (i = 0; i < CAN1_DLC; i++) {
        TxData[i] = CAN1Sedbuf[i];
    }

    if (HAL_CAN_GetTxMailboxesFreeLevel(&SYNTRON_CAN_MOTOR_H) == 0) {
        return 1;
    }

    if (HAL_CAN_AddTxMessage(&SYNTRON_CAN_MOTOR_H, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
        // log_d("Motor speed send success.");
        return 0;
    } else {
        // log_e("Motor speed send error.");
        return 1;
    }
}

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR_fengdekong */
/* @} Robot_BSP */
