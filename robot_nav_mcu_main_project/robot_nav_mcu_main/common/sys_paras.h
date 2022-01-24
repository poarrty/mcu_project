/*************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:      sys_paras.h
  ** Author:
  ** Version:
  ** Date:
  ** Description:
  ** Others:
  ** Function List:
  ** History:

  ** <time>   <author>    <version >   <desc>
  **
*************************************************/

#ifndef __SYS_PARAS_H__
#define __SYS_PARAS_H__
#include "main.h"

#include "sys_exc.h"

#pragma pack(1)

typedef struct {
    uint32_t motor_max_current_output;  ///< 最大输出电流阈值   0.1A
    uint32_t motor_min_current_output;  ///< 最小输出电流阈值   0.1A

    uint32_t motor_warn_current;         ///< 电机电流预警值
    uint32_t motor_normal_current;       ///< 电机电流正常值
    uint32_t motor_current_sample_time;  ///< 电机电流采样时间(秒)

} motor_current_set_t;
typedef struct {
    uint32_t id;                         ///< 设备ID 暂时没用到
    uint32_t wwdg_timeout_coredown_cnt;  ///< 窗口看门狗超时次数
    uint32_t hardfault_coredown_cnt;     ///< 硬件错误次数

    uint32_t auto_on_time_ms;          ///< 上电后自动开机时间 为 0XFFFFFFFF则不自动开机
    uint32_t nvg_rk_up_timeout_ms;     ///< 导航板 RK 开机超时时间/ms
    uint32_t nvg_rk_down_timeout_ms;   ///< 导航板 RK 关机超时时间/ms
    uint32_t task_rk_up_timeout_ms;    ///< 任务板 RK 开机超时时间/ms
    uint32_t task_rk_down_timeout_ms;  ///< 任务板 RK 关机超时时间/ms

    uint32_t lamp_shake_period;    ///< 开关机灯光闪烁周期/ms
    uint32_t emerg_shake_period;   ///< 急停灯闪烁周期/ms
    uint32_t button_shake_period;  ///< 按键防抖时间/ms

    uint32_t left_color_r;  ///< 机器人示廓灯颜色
    uint32_t left_color_g;
    uint32_t left_color_b;
    uint32_t right_color_r;
    uint32_t right_color_g;
    uint32_t right_color_b;

    uint32_t epb_delay_ms;  ///< 平地刹车延迟启动

    uint32_t imu_pitch_threshold;  ///< 机器人俯仰角阈值(绝对值乘以10)
    uint32_t luna_delt_threshold;  ///< 机器人单点激光测距差阈值
    uint32_t luna_dis_normal;      ///< 机器人单点激光平地测距值
    uint32_t luna_dis_min;
    uint32_t luna_dis_max;          ///< 机器人断崖阈值
    uint32_t imu_bump_smp_inval;    ///< 机器人颠簸检测IMU采样间隔
    uint32_t luna_smp_inval;        ///< 机器人单点激光测距采样间隔
    uint32_t bump_up_hold_time_ms;  ///< 机器人测距和俯仰角有效持续时间

    uint32_t wheel_diameter;                    ///< 车轮直径/mm
    uint32_t wheel_space;                       ///< 车轮间距/mm
    uint32_t motor_drive_type;                  ///< 电机驱动类型            0X01: 自研驱动器
                                                ///< 0X02: 森创驱动器
    uint32_t            motor_acc_up;           ///< 电机控制上升加速度      mm/s^2
    uint32_t            motor_acc_down;         ///< 电机控制下降加速度      mm/s^2
    uint32_t            motor_reduction_ratio;  ///< 电机减速比
    uint32_t            motor_pole;             ///< 电机极对数
    uint32_t            motor_inc_per_round;    ///< 电机转一圈位置增加总量
    uint32_t            motor_rpm_max;          ///< 电机最大转速
    uint32_t            motor_mmps_max;         ///< 最大速度 mm/s
    motor_current_set_t motor_current;

    uint32_t auto_charge_loc_update_ms;  ///< 自动充电位置更新周期

    uint32_t battery_low_protect;  ///< 低电量保护触发百分比，0则不保护强制关机

    uint32_t sys_exc_detect_enable;  ///< 异常检测总开关
    uint8_t  sys_exc_mask[EXC_NUM / 8];

    uint16_t crc;

} SYS_PARAS_T, *SYS_PARAS_P;

#pragma pack()

typedef enum {
    SYS_PARAS_NULL,
    SYS_PARAS_ID,
    SYS_PARAS_AUTO_ON_TIME,
    SYS_PARAS_NRK_UP_TO,
    SYS_PARAS_NRK_DOWN_TO,
    SYS_PARAS_TRK_UP_TO,
    SYS_PARAS_TRK_DOWN_TO,
    SYS_PARAS_LAMP_PERIOD,
    SYS_PARAS_EMERG_PERIOD,
    SYS_PARAS_BUTTON_PERIOD,
    SYS_PARAS_LAMP_COLOR,
    SYS_PARAS_EPB_DELAY,
    SYS_PARAS_IMU_PITCH_H,
    SYS_PARAS_LUNA_DELT_H,
    SYS_PARAS_DIS_NOR,
    SYS_PARAS_DIS_MIN,
    SYS_PARAS_DIS_MAX,
    SYS_PARAS_IMU_BUMP_SMP_INV,
    SYS_PARAS_LUNA_SMP_INV,
    SYS_PARAS_BUMP_HOLD_TIME,
    SYS_PARAS_WHEEL_DIAMETER,
    SYS_PARAS_WHEEL_SPACE,
    SYS_PARAS_MOTOR_DRIVE_TYPE,
    SYS_PARAS_MOTOR_ACC_UP,
    SYS_PARAS_MOTOR_ACC_DOWN,
    SYS_PARAS_MOTOR_REDUCTION_RATIO,
    SYS_PARAS_MOTOR_POLE,
    SYS_PARAS_MOTOR_INC_PER_ROUND,
    SYS_PARAS_MOTOR_RPM_MAX,
    SYS_PARAS_MOTOR_MMPS_MAX,
    SYS_PARAS_AUTO_CHARGE_POS_UPDATE_PERIOD,
    SYS_PARAS_BATTERT_LOW_PROTECT,
    SYS_PARAS_EXC_DETECT_ENABLE

} PARAS_ITEM_E;

typedef enum {
    MOTOR_DRIVE_SELF       = 1,
    MOTOR_DRIVE_SYNTRON    = 2,
    MOTOR_DRIVE_FENGDEKONG = 3,
    MOTOR_DRIVE_TAIZHAO    = 4,

} MOTOR_DRIVE_TYPE_E;

extern SYS_PARAS_T sys_paras;

uint8_t sys_paras_init(void);                             ///< 系统配置参数初始化
uint8_t sys_paras_show(void);                             ///< 系统配置参数打印
uint8_t sys_paras_read(SYS_PARAS_T *paras);               ///< 系统配置参数读取
uint8_t sys_paras_write(void);                            ///< 系统配置参数写入
uint8_t sys_paras_write_def(void);                        ///< 系统配置参数写入默认参数
uint8_t sys_paras_write_id(uint32_t val);                 ///< 写入 设备ID
uint8_t sys_paras_write_epb_delay(uint32_t val);          ///< 写入 刹车延迟时间
uint8_t sys_paras_write_nvg_up_timeout(uint32_t val);     ///< 写入 导航板开机超时时间
uint8_t sys_paras_write_nvg_down_timeout(uint32_t val);   ///< 写入 导航板关机超时时间
uint8_t sys_paras_write_task_up_timeout(uint32_t val);    ///< 写入 任务板开机超时时间
uint8_t sys_paras_write_task_down_timeout(uint32_t val);  ///< 写入 任务板开机超时时间
uint8_t sys_paras_write_imu_pitch_threshold(uint32_t val);
uint8_t sys_paras_write_luna_delt_threshold(uint32_t val);
uint8_t sys_paras_write_luna_dis_normal(uint32_t val);
uint8_t sys_paras_write_luna_dis_max(uint32_t val);
uint8_t sys_paras_write_luna_smp_inval(uint32_t val);
uint8_t sys_paras_write_bump_up_hold_time_ms(uint32_t val);
uint8_t sys_paras_write_lamp_shake_period(uint32_t val);
uint8_t sys_paras_write_emerg_shake_period(uint32_t val);
uint8_t sys_paras_write_button_shake_period(uint32_t val);
uint8_t sys_paras_write_lamp_color(uint32_t val);
uint8_t sys_paras_set(PARAS_ITEM_E item, uint32_t val);
uint8_t sys_paras_use_def(void);

uint8_t sys_paras_write_coredown(char *msg, uint32_t len);
uint8_t sys_paras_read_coredown(char *msg);
uint8_t sys_paras_wwdg_to_inc(void);
uint8_t sys_paras_hardfault_inc(void);

#endif
