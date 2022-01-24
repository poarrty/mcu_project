/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-22
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-5-22       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_motor_syntron.h"
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
#define SDO_W1          0x2F
#define SDO_W2          0x2B
#define SDO_W4          0x23
#define SDO_RD          0x40
#define SDO_W_TWO_MOTOR 0xE9

// CAN ID

#define Left_Wheel_ID  0x0002
#define Right_Wheel_ID 0x0001

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

uint8_t syntron_can_buff[8];

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
void CANopen_PV_Init(void);
void Motor_enable(void);
u8   SDO_Write_OD(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA);
u8   SDO_Write_OD_Ndly(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA);
void bsp_syntron_speed_set(u8 CANopen_ID, u32 DATA);

static u8 CAN1_Send(u16 Id, u8 CAN1_DLC);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_motor_syntron_init
 * Description:    初始化 电机控制 森创驱动器
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
int bsp_motor_syntron_init(void) {
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
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_motor_syntron_init, bsp_motor_syntron_init,
                 bsp_motor_syntron_init);

/*****************************************************************/
/**
 * Function:       bsp_motor_syntron_deInit
 * Description:    释放 bsp_motor_syntron 资源
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
int bsp_motor_syntron_deInit(void) {
    return 0;
}

static void syntron_motor_delay(uint8_t ticks) {
    if (osKernelGetState() == osKernelRunning) {
        osDelay(ticks);
    } else {
        HAL_Delay(ticks);
    }
}

/*****************************************************************/
/**
 * Function:       bsp_motor_syntron_set_speed
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
uint8_t bsp_motor_syntron_set_speed(float speed_l, float speed_r, float reduction, MOTOR_LOCK_TYPE_E lock) {
    int16_t rpm_min, rpm_max;

    // return 0;

    ///< 左右轮转向定义相反
    speed_r = speed_r * -1;

    rpm_min = -1 * (int32_t) sys_paras.motor_rpm_max;
    rpm_max = 1 * sys_paras.motor_rpm_max;

    ///< 底层限速
    speed_l = (speed_l > rpm_max) ? (rpm_max) : (speed_l);
    speed_l = (speed_l < rpm_min) ? (rpm_min) : (speed_l);

    speed_r = (speed_r > rpm_max) ? (rpm_max) : (speed_r);
    speed_r = (speed_r < rpm_min) ? (rpm_min) : (speed_r);

    speed_l = speed_l * reduction * 10;
    speed_r = speed_r * reduction * 10;

    log_d("Send motor syntron drv l[%d] r[%d]", (int32_t) speed_l, (int32_t) speed_r);

    bsp_syntron_speed_set(Left_Wheel_ID, (int32_t) speed_l);
    syntron_motor_delay(5);
    bsp_syntron_speed_set(Right_Wheel_ID, (int32_t) speed_r);

    return 0;
}

uint8_t bsp_motor_syntron_req_pos(void) {
    SDO_Write_OD_Ndly(Left_Wheel_ID, SDO_RD, 0x6063, 0x00, 0);
    SDO_Write_OD_Ndly(Right_Wheel_ID, SDO_RD, 0x6063, 0x00, 0);

    return 0;
}

u8 CAN1Sedbuf[8];
u8 CAN1Redbuf[8];

//速度模式PV：
u32 PV_spd;

/*模式设置*/
u8 Contol_Mode_SET(u8 CANopen_ID, u8 CANopen_mode) {
    CAN1Sedbuf[0] = SDO_W1;
    CAN1Sedbuf[1] = 0x60;
    CAN1Sedbuf[2] = 0x60;
    CAN1Sedbuf[3] = 0x00;
    CAN1Sedbuf[4] = CANopen_mode;
    CAN1Sedbuf[5] = 0x00;
    CAN1Sedbuf[6] = 0x00;
    CAN1Sedbuf[7] = 0x00;
    CAN1_Send(0x600 + CANopen_ID, 8);
    delay_ms(5);
    return (1);
}
/*激活节点*/
u8 CANopen_Activate(u8 CANopen_ID) {
    CAN1Sedbuf[0] = 0x01;
    CAN1Sedbuf[1] = CANopen_ID;
    CAN1_Send(0x000, 2);
    delay_ms(5);
    return (1);
}
#if 0
u8 SDO_Write_OD(u8 CANopen_ID,u8 CMD, u16 Index, u8 SubIndex, u32 DATA){

    CAN1Sedbuf[0]=CMD;
    CAN1Sedbuf[1]=(u8)(Index    & 0xFF);
    CAN1Sedbuf[2]=(u8)(Index>>8 & 0xFF);
    CAN1Sedbuf[3]=SubIndex;
    CAN1Sedbuf[4]=(u8)(DATA     & 0xFF);
    CAN1Sedbuf[5]=(u8)(DATA>>8  & 0xFF);
    CAN1Sedbuf[6]=(u8)(DATA>>16 & 0xFF);
    CAN1Sedbuf[7]=(u8)(DATA>>24 & 0xFF);	
    CAN1_Send(0x600 + CANopen_ID,8);
    
    syntron_motor_delay(5);
    return(1);
}
#endif
u8 SDO_Write_OD(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA) {
    uint8_t times = 5;

    memset(syntron_can_buff, 0, sizeof(syntron_can_buff));

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
            syntron_motor_delay(1);

            if (memcmp(&syntron_can_buff[1], &CAN1Sedbuf[1], sizeof(syntron_can_buff) - 1) == 0) {
                return 1;
            }
        }

        times--;
    }

    log_d("CAN error...!\r\n");

    return 0;
}

u8 SDO_Write_OD_Ndly(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex, u32 DATA) {
    CAN1Sedbuf[0] = CMD;
    CAN1Sedbuf[1] = (u8)(Index & 0xFF);
    CAN1Sedbuf[2] = (u8)(Index >> 8 & 0xFF);
    CAN1Sedbuf[3] = SubIndex;
    CAN1Sedbuf[4] = (u8)(DATA & 0xFF);
    CAN1Sedbuf[5] = (u8)(DATA >> 8 & 0xFF);
    CAN1Sedbuf[6] = (u8)(DATA >> 16 & 0xFF);
    CAN1Sedbuf[7] = (u8)(DATA >> 24 & 0xFF);
    CAN1_Send(0x600 + CANopen_ID, 8);
    // delay_ms(5);
    return (1);
}
void bsp_syntron_speed_set(u8 CANopen_ID, u32 DATA) {
    CAN1Sedbuf[0] = (u8)(DATA & 0xFF);
    CAN1Sedbuf[1] = (u8)(DATA >> 8 & 0xFF);
    CAN1Sedbuf[2] = (u8)(DATA >> 16 & 0xFF);
    CAN1Sedbuf[3] = (u8)(DATA >> 24 & 0xFF);
    CAN1_Send(0x400 + CANopen_ID, 4);
}

u32 SDO_Read_OD(u8 CANopen_ID, u8 CMD, u16 Index, u8 SubIndex) {
    return (1);
}
// PV canopen设置

void CANopen_PV_Init(void) {
    // STEP1:激活节点1、节点2
    CANopen_Activate(Left_Wheel_ID);
    CANopen_Activate(Right_Wheel_ID);

    // STEP2:设置速度模式  6060H写为3
    Contol_Mode_SET(Left_Wheel_ID, PV_Mode);
    Contol_Mode_SET(Right_Wheel_ID, PV_Mode);

    // STEP3:设置加减速 写6083H和6084H
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x6083, 0x00, 200000 - sys_paras.motor_acc_up * 1000);
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x6084, 0x00, 200000 - sys_paras.motor_acc_down * 1000);
    // SDO_Write_OD( Left_Wheel_ID, SDO_W4, 0x6084,0x00,0x000003e8);

    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x6083, 0x00, 200000 - sys_paras.motor_acc_up * 1000);
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x6084, 0x00, 200000 - sys_paras.motor_acc_down * 1000);
    // SDO_Write_OD( Right_Wheel_ID,SDO_W4, 0x6084,0x00,0x000003e8);

    // STEP4:设置目标转速为0  写60FFH

    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x60FF, 0x00, 0x00000000);
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x60FF, 0x00, 0x00000000);

    // STEP5:设置位置反馈

    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1A00, 0x00, 0x00000000);  // TPDO1
                                                                    // STOP

    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x1A00, 0x01,
                 0x60630020);  //码盘位置对应对象字典 0x6063
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x1A00, 0x02,
                 0x60690020);  //实际转速对应对象字典 0x6069
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1A00, 0x00,
                 0x00000002);                                // TPDO1 ENABLE. WITH 2 item
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1800, 0x05, 20);   // PERIOD 20 ms
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1800, 0x03, 20);   // PERIOD 20 ms
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1800, 0x02, 254);  // PERIOD UPLOAD

    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1A00, 0x00,
                 0x00000000);  // TPDO1 STOP.
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x1A00, 0x01,
                 0x60630020);  //码盘位置对应对象字典 0x6063
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x1A00, 0x02,
                 0x60690020);  //实际转速对应对象字典 0x6069
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1A00, 0x00,
                 0x00000002);                                 // TPDO1 ENABLE. WITH 2 item
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1800, 0x05, 20);   // PERIOD 20 ms
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1800, 0x03, 20);   // PERIOD 20 ms
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1800, 0x02, 254);  // PERIOD UPLOAD

    ///<电机错误码,负载率
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1A01, 0x00, 0x00000000);  // TPDO2
                                                                    // STOP
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x1A01, 0x01,
                 0x51120020);  //电机状态对应对象字典 0x5012
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x1A01, 0x02,
                 0x51020010);  //电机负载率对应对象字典 0x5002
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1A01, 0x00,
                 0x00000002);                                // TPDO2 ENABLE. WITH 2 item
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1801, 0x05, 50);   // PERIOD 20 ms
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1801, 0x03, 50);   // PERIOD 20 ms
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1801, 0x02, 254);  // PERIOD UPLOAD

    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1A01, 0x00,
                 0x00000000);  // TPDO2 STOP
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x1A01, 0x01,
                 0x50120010);  //报警码对应对象字典 5012
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x1A01, 0x02,
                 0x50020010);  //电机负载率对应对象字典 0x5002
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1A01, 0x00,
                 0x00000002);                                 // TPDO2 ENABLE. WITH 2 item
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1801, 0x05, 50);   // PERIOD 50 ms
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1801, 0x03, 50);   // PERIOD 50 ms
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1801, 0x02, 254);  // PERIOD UPLOAD

    ///<速度下发RPDO3
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1602, 0x00, 0x00000000);  // RPDO3
                                                                    // STOP
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x1602, 0x01, 0x60FF0010);  // 0x60FF
    SDO_Write_OD(Left_Wheel_ID, SDO_W1, 0x1602, 0x00,
                 0x00000001);  // RPDO3 ENABLE

    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1602, 0x00,
                 0x00000000);                                        // RPDO3 STOP
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x1602, 0x01, 0x60FF0010);  // 0x60FF
    SDO_Write_OD(Right_Wheel_ID, SDO_W1, 0x1602, 0x00,
                 0x00000001);  // RPDO3 ENABLE

    //<速度设置为0
    bsp_syntron_speed_set(Left_Wheel_ID, (int32_t) 0);
    syntron_motor_delay(5);
    bsp_syntron_speed_set(Right_Wheel_ID, (int32_t) 0);

    // STEP6 心跳间隔
    SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x1017, 0x00, 100);

    bsp_syntron_motor_max_current_output(200);  ///<最大输出电流20A
}

void CANopen_PV_SET(u32 Acc, u32 Dec, s32 TargetVelocity) {
    // STEP1:设置加减速 写6083H和6084H
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x6083, 0x00, Acc);
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x6084, 0x00, Dec);

    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x6083, 0x00, Acc);
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x6084, 0x00, Dec);

    // STEP2:设置目标转速  写60FFH
    SDO_Write_OD(Left_Wheel_ID, SDO_W4, 0x60FF, 0x00, TargetVelocity);
    SDO_Write_OD(Right_Wheel_ID, SDO_W4, 0x60FF, 0x00, TargetVelocity);
}

void Motor_enable(void) {
    //使能写0x6040分别为6、7、F

    SDO_Write_OD(Left_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x06);
    SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x06);

    SDO_Write_OD(Left_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x07);
    SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x07);

    SDO_Write_OD(Left_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x0F);
    SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x0F);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), syntron_enable, Motor_enable, syntron_enable);

uint8_t Motor_Disable(void) {
    //失能写0x6040分别为7
    if (SDO_Write_OD(Left_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x07) != 1) {
        return 1;
    }

    if (SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x07) != 1) {
        return 1;
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), syntron_disable, Motor_Disable, syntron_disable);

uint8_t bsp_syntron_motor_enable(void) {
    if (SDO_Write_OD(Left_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x0F) != 1) {
        return 1;
    }

    if (SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x6040, 0x00, 0x0F) != 1) {
        return 1;
    }

    return 0;
}

/*
 * Function:       bsp_syntron_motor_max_current_output
 * Description:    设置轮毂电机最大输出电流
 * Calls:
 * Called By:
 * @param[in]      @value:最大输出电流值，单位0.1A,数据有效范围50~400
 */
void bsp_syntron_motor_max_current_output(uint16_t value) {
    uint32_t max_current = value * 10;
    if ((max_current >= 500) && (max_current <= 4000)) {
        SDO_Write_OD(Right_Wheel_ID, 0xe9, 0x242A, 0, (max_current << 16) | max_current);
        SDO_Write_OD(Right_Wheel_ID, 0xe9, 0x242A, 0, (max_current << 16) | max_current);
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), syntron_max_current_output,
                 bsp_syntron_motor_max_current_output, syntron_max_current_output);

/*电机驱动器复位*/
void bsp_syntron_motor_reset(void) {
    SDO_Write_OD(Right_Wheel_ID, SDO_W2, 0x4603, 0, 0x01);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), syntron_motor_reset, bsp_syntron_motor_reset,
                 syntron_motor_reset);

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
        while((CAN_TransmitStatus(CAN1,
    mail)==CAN_TxStatus_Failed)&&(i<0XFFF))i++; //等待发送结束
    if(i>=0XFFF)return 1; return 0;
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

void set_syntron_msg(uint8_t *buff) {
    memcpy(syntron_can_buff, buff, sizeof(syntron_can_buff));
}

#ifdef __cplusplus
}
#endif

/* @} BSP_MOTOR_SYNTRON */
/* @} Robot_BSP */
