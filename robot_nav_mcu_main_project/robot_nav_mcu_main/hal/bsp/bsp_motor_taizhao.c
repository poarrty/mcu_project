/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         linzhongxing
 ** Version:        V0.0.1
 ** Date:           2021-9-18
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-09 linzhongxing 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-9-18       linzhongxing    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_motor_taizhao.h"
#include "can.h"
#include "shell.h"
#include "sys_exc.h"
#include "sys_paras.h"
#include "typedef.h"
#include "cmsis_os.h"
#include "string.h"

#define LOG_TAG "bsp_motor"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_MOTOR_SYNTRON -
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

#define LEFT_ID  0x0002
#define RIGHT_ID 0x0001

// Object dictionary of CANopen
#define Heartbeat_interval           0x1017
#define Control_word                 0x6040
#define Status_word                  0x6041
#define Modes_of_operation           0x6060
#define Modes_0f_operation_display   0x6061
#define Position_actual_value        0x6063
#define Velocity_sensor_actual_value 0x6069
#define Velocity_actual_value        0x606C
#define Target_torque                0x6071
#define MOTOR_MAX_CURRENT            0x6073
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
uint8_t sdo_resp_msg[3];

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

extern bool motor_enable_flag;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void bsp_taizhao_motor_init(void);
static void bsp_taizhao_motor_enable_init(void);
static u8   SDO_Write_OD(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA);

static u8 CAN1_Send(u16 Id, u8 CAN1_DLC);
static u8 set_speed(uint32_t left_speed, uint32_t right_speed, uint8_t enable_flag);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:
 * Description:    初始化电机控制
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
int bsp_motor_taizhao_init(void) {
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

        if (HAL_CAN_ConfigFilter(&MOTOR_CAN_H, &sFilterConfig) == HAL_OK) {
            log_i("can filter success!\r\n");
        } else {
            log_e("can filter error[0X%X]!\r\n", MOTOR_CAN_H.ErrorCode);
            /* Filter configuration Error */
            Error_Handler();
        }

        if (HAL_CAN_Start(&MOTOR_CAN_H) == HAL_OK) {
            log_i("can start success!\r\n");
            HAL_CAN_ActivateNotification(&MOTOR_CAN_H, CAN_IT_RX_FIFO0_MSG_PENDING);
        } else {
            log_e("can start error!\r\n");
            /* Start Error */
            Error_Handler();
        }

        can_init_flag = 1;
    }

    bsp_taizhao_motor_init();
    bsp_taizhao_motor_enable_init();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_motor_taizhao_init, bsp_motor_taizhao_init,
                 bsp_motor_taizhao_init);

/*****************************************************************/
/**
 * Function:       bsp_motor_taizhao_deInit
 * Description:    释放 bsp_motor_taizhao_deInit 资源
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
int bsp_motor_taizhao_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_motor_taizhao_set_speed
 * Description:    设置电机速度
 * Calls:
 * Called By:
 * @param[in]      speed_l: uint 0.1 rpm （非轮速，是电机内部转速，到轮子要有减速比）
 * @param[in]      speed_r: unit 0.1 rpm （非轮速，是电机内部转速，到轮子要有减速比）
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
uint8_t bsp_motor_taizhao_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock) {
    int16_t rpm_min, rpm_max;

    ///< 左右轮转向定义相反
    speed_r = speed_r * -1;

    rpm_min = -1 * (int32_t) sys_paras.motor_rpm_max;
    rpm_max = 1 * sys_paras.motor_rpm_max;

    ///< 底层限速
    speed_l = (speed_l > rpm_max) ? (rpm_max) : (speed_l);
    speed_l = (speed_l < rpm_min) ? (rpm_min) : (speed_l);

    speed_r = (speed_r > rpm_max) ? (rpm_max) : (speed_r);
    speed_r = (speed_r < rpm_min) ? (rpm_min) : (speed_r);
#if 1
    speed_l = speed_l * reduction * 10;
    speed_r = speed_r * reduction * 10;
#endif
    log_d("Send motor taizhao drv l[%d] r[%d][%d]", (int32_t) speed_l, (int32_t) speed_r, (uint8_t) motor_enable_flag);

    set_speed((int32_t) speed_l, (int32_t) speed_r, (uint8_t) motor_enable_flag);

    return 0;
}

static void taizhao_delay(uint8_t value) {
    if (osKernelGetState() == osKernelRunning) {
        osDelay(value);
    } else {
        delay_ms(value);
    }
}

static u8 CAN1Sedbuf[8];

#if 0
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
#endif

/*激活节点*/
static u8 CANopen_Activate(u8 CANopen_ID) {
    CAN1Sedbuf[0] = 0x01;
    CAN1Sedbuf[1] = CANopen_ID;
    CAN1_Send(0x000, 2);
    taizhao_delay(20);
    return (1);
}

static u8 SDO_Write_OD(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA) {
    uint8_t times = 5;

    memset(sdo_resp_msg, 0, sizeof(sdo_resp_msg));

    while (times > 0)  ///<重发
    {
        CAN1Sedbuf[0] = CMD;
        CAN1Sedbuf[1] = (u8)(Index & 0xFF);
        CAN1Sedbuf[2] = (u8)(Index >> 8 & 0xFF);
        CAN1Sedbuf[3] = SubIndex;
        CAN1Sedbuf[4] = (u8)(DATA & 0xFF);
        CAN1Sedbuf[5] = (u8)(DATA >> 8 & 0xFF);
        CAN1Sedbuf[6] = (u8)(DATA >> 16 & 0xFF);
        CAN1Sedbuf[7] = (u8)(DATA >> 24 & 0xFF);
        CAN1_Send(0x600 + CANopen_ID, 8);

        for (uint8_t wait_cnt = 0; wait_cnt < 5; wait_cnt++) {
            taizhao_delay(2);

            if ((sdo_resp_msg[0] == 0x60) && (memcmp(&sdo_resp_msg[1], &CAN1Sedbuf[1], sizeof(sdo_resp_msg) - 1) == 0)) {
                return 1;
            }
        }

        times--;
    }

    log_e("CAN error...!\r\n");

    return 0;
}

static u8 set_speed(uint32_t left_speed, uint32_t right_speed, uint8_t enable_flag) {
    uint8_t control_world = 0x0f;

    if (enable_flag == 0) {
        control_world = 0x07;
    }

    CAN1Sedbuf[0] = control_world;
    CAN1Sedbuf[1] = 0x00;
    CAN1Sedbuf[2] = (u8)(left_speed & 0xff);
    CAN1Sedbuf[3] = (u8)(left_speed >> 8 & 0xFF);
    CAN1Sedbuf[4] = (u8)(left_speed >> 16 & 0xFF);
    CAN1Sedbuf[5] = (u8)(left_speed >> 24 & 0xFF);
    CAN1_Send(COBID_SPEED_L, 6);

    // taizhao_delay(5);

    CAN1Sedbuf[0] = control_world;
    CAN1Sedbuf[1] = 0x00;
    CAN1Sedbuf[2] = (u8)(right_speed & 0xff);
    CAN1Sedbuf[3] = (u8)(right_speed >> 8 & 0xFF);
    CAN1Sedbuf[4] = (u8)(right_speed >> 16 & 0xFF);
    CAN1Sedbuf[5] = (u8)(right_speed >> 24 & 0xFF);
    CAN1_Send(COBID_SPEED_R, 6);

    CAN1_Send(COBID_SYNC, 0);

    return (1);
}
#if 0
#define PI 3.1415926f
static uint16_t acc_time_calc(int rate, float acc, int diameter) {
    /*计算公式
    R：额定转速(r/min)
    T：到达额定转速时间(ms)
    A:加速度(m/s2)
    π*L*R/60/T = A;
       */
    uint16_t time = (1000.0 * diameter * rate * PI) / (acc * 60);
    return time;
}
#endif
static void bsp_taizhao_motor_init(void) {
    // STEP1:激活节点1
    CANopen_Activate(RIGHT_ID);
    CANopen_Activate(LEFT_ID);

    // STEP2:设置速度模式	6060H写为3
    //    Contol_Mode_SET( PV_Mode );

    // STEP3:设置加减速	写6083H和6084H
#if 0
   
    uint16_t acc_time = acc_time_calc(sys_paras.motor_rpm_max,sys_paras.motor_acc_up,sys_paras.wheel_diameter);
    SDO_Write_OD(  SDO_W2, 0x6083,0x01,acc_time);
    SDO_Write_OD(  SDO_W2, 0x6883,0x01,acc_time);
    
    acc_time = acc_time_calc(sys_paras.motor_rpm_max,sys_paras.motor_acc_down,sys_paras.wheel_diameter);
    SDO_Write_OD(  SDO_W2, 0x6084,0x01,acc_time);
    SDO_Write_OD(  SDO_W2, 0x6884,0x01,acc_time);

#endif

    // STEP4:设置目标转速为0	写60FFH

#if 0

    SDO_Write_OD(LEFT_ID, SDO_W1, 0x1401, 0x02, 0x01);
    //保存FLASH
    SDO_Write_OD(LEFT_ID, SDO_W1, 0x1010, 0x01, 0x65766173);

    SDO_Write_OD(RIGHT_ID, SDO_W1, 0x1401, 0x02, 0x01);
    //保存FLASH
    SDO_Write_OD(RIGHT_ID, SDO_W1, 0x1010, 0x01, 0x65766173);

#endif
    SDO_Write_OD(LEFT_ID, SDO_W2, Heartbeat_interval, 0x00, 100);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Heartbeat_interval, 0x00, 100);
}

static void bsp_taizhao_motor_enable_init(void) {
    //清除错误
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x80);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x80);

    //使能写0x6040分别为6、7、F
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x06);
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x07);
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x0F);

    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x06);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x07);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x0F);
}

uint8_t bsp_taizhao_motor_disable(void) {
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x07);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x07);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), taizhao_disable, bsp_taizhao_motor_disable,
                 bsp_taizhao_motor_disable);

uint8_t bsp_taizhao_motor_enable(void) {
    SDO_Write_OD(LEFT_ID, SDO_W2, Control_word, 0x00, 0x0F);
    SDO_Write_OD(RIGHT_ID, SDO_W2, Control_word, 0x00, 0x0F);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), taizhao_enable, bsp_taizhao_motor_enable,
                 bsp_taizhao_motor_enable);

static u8 CAN1_Send(u16 Id, u8 CAN1_DLC) {
    /*
                u8 mail;
                u16 i=0;
                TxMes1.StdId = Id;	         // 标准标识符ID
                TxMes1.ExtId = Id;	         // 扩展标示符ID（29位）
                TxMes1.IDE = CAN_Id_Standard; // 使用标准贞
                TxMes1.RTR = CAN_RTR_Data;		 // 消息类型为数据帧；/遥控帧
                TxMes1.DLC = CAN1_DLC;		         // 	1·报文长度
    for(i=0;i<CAN1_DLC;i++)
          TxMes1.Data[i]=CAN1Sedbuf[i];				 // 第一帧信息
                mail = CAN_Transmit(CAN1, &TxMes1);
                i=0;
                while((CAN_TransmitStatus(CAN1, mail)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
                if(i>=0XFFF)return 1;
                return 0;
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

    if (HAL_CAN_GetTxMailboxesFreeLevel(&MOTOR_CAN_H) == 0)
        return 1;

    if (HAL_CAN_AddTxMessage(&MOTOR_CAN_H, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
        // log_i("Motor speed send success.");
        return 0;
    } else {
        // log_e("Motor speed send error.");
        return 1;
    }
}

void taizhao_sdo_resp_msg(uint8_t *buff) {
    memcpy(sdo_resp_msg, buff, sizeof(sdo_resp_msg));
}
#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR_SYNTRON */
/* @} Robot_BSP */
