/*************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:      sys_paras.c
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
#include "sys_paras.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "shell.h"
#include "string.h"
#include "elog.h"
#include "bsp_led.h"
#include "rtc.h"
#include "crc16.h"
#include "wwdg.h"
#include "flashdb.h"

///< 安保机器人默认参数
#define DEF_AUTO_ON_TIME_MS_SC       0XFFFFFFFF
#define DEF_WHEEL_DIAMETER_SC        317               ///< 车轮直径/mm
#define DEF_WHEEL_SPACE_SC           588               ///< 车轮间距/mm
#define DEF_MOTOR_DRIVE_TYPE_SC      MOTOR_DRIVE_SELF  ///< 电机驱动类型
#define DEF_MOTOR_ACC_UP_SC          10                ///< 上升加速度 mm/s^2
#define DEF_MOTOR_ACC_DOWN_SC        50                ///< 下降加速度 mm/s^2
#define DEF_MOTOR_REDUCTION_RATIO_SC 165               ///< 电机减速比 16.5
#define DEF_MOTOR_POLE_SC            4                 ///< 电机极对数(暂时没用到）
#define DEF_MOTOR_INC_PER_ROUND_SC   600               ///< 电机转一圈位置增加总量
#define DEF_MOTOR_RPM_MAX_SC         25000             ///< 电机最大转速
#define DEF_MOTOR_MMPS_MAX_SC        2000              ///< 最大速度

///< 清洁机器人默认参数

///< 自研电机
#define DEF_AUTO_ON_TIME_MS_CL_SELF       1500
#define DEF_WHEEL_DIAMETER_CL_SELF        170               ///< 车轮直径/mm
#define DEF_WHEEL_SPACE_CL_SELF           530               ///< 车轮间距/mm
#define DEF_MOTOR_DRIVE_TYPE_CL_SELF      MOTOR_DRIVE_SELF  ///< 电机驱动类型
#define DEF_MOTOR_ACC_UP_CL_SELF          10                ///< 上升加速度 mm/s^2
#define DEF_MOTOR_ACC_DOWN_CL_SELF        48                ///< 下降加速度 mm/s^2
#define DEF_MOTOR_REDUCTION_RATIO_CL_SELF 100               ///< 电机减速比 1
#define DEF_MOTOR_POLE_CL_SELF            1                 ///< 电机极对数(暂时没用到）
#define DEF_MOTOR_INC_PER_ROUND_CL_SELF   16384             ///< 电机转一圈位置增加总量
#define DEF_MOTOR_RPM_MAX_CL_SELF         25000             ///< 电机最大转速
#define DEF_MOTOR_MMPS_MAX_CL_SELF        2000              ///< 最大速度

#define DEF_AUTO_ON_TIME_MS_CL                  1500
#define DEF_WHEEL_DIAMETER_CL                   169                  ///< 车轮直径/mm
#define DEF_WHEEL_SPACE_CL                      530                  ///< 车轮间距/mm
#define DEF_MOTOR_DRIVE_TYPE_CL                 MOTOR_DRIVE_SYNTRON  ///< 电机驱动类型
#define DEF_MOTOR_ACC_UP_CL                     150                  ///< 上升加速度 mm/s^2
#define DEF_MOTOR_ACC_DOWN_CL                   190                  ///< 下降加速度 mm/s^2
#define DEF_MOTOR_REDUCTION_RATIO_CL            10                   ///< 电机减速比 1
#define DEF_MOTOR_POLE_CL                       1                    ///< 电机极对数(暂时没用到）
#define DEF_MOTOR_INC_PER_ROUND_CL              5600                 ///< 电机转一圈位置增加总量
#define DEF_MOTOR_RPM_MAX_CL                    3000                 ///< 电机最大转速
#define DEF_MOTOR_MMPS_MAX_CL                   5000                 ///< 最大速度
#define DEF_MOTOR_CURRENT_SAMPLE_TIME_CL        30                   ///< 30S
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX_CL 200                  ///< 最大20A
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN_CL 100                  ///< 最小10A
#define DEF_MOTOR_SYNTRON_CURRENT_WARN_CL       140                  ///< 采样电流=最大输出电流*0.7
#define DEF_MOTOR_SYNTRON_CURRENT_NORMAL_CL     50                   ///< 正常电流

#define DEF_AUTO_ON_TIME_MS_CL_FENGDEKONG       1500
#define DEF_WHEEL_DIAMETER_CL_FENGDEKONG        150                     ///< 车轮直径/mm
#define DEF_WHEEL_SPACE_CL_FENGDEKONG           530                     ///< 车轮间距/mm
#define DEF_MOTOR_DRIVE_TYPE_CL_FENGDEKONG      MOTOR_DRIVE_FENGDEKONG  ///< 电机驱动类型
#define DEF_MOTOR_ACC_UP_CL_FENGDEKONG          150                     ///< 上升加速度 mm/s^2
#define DEF_MOTOR_ACC_DOWN_CL_FENGDEKONG        190                     ///< 下降加速度 mm/s^2+
#define DEF_MOTOR_REDUCTION_RATIO_CL_FENGDEKONG 10                      ///< 电机减速比 1
#define DEF_MOTOR_POLE_CL_FENGDEKONG            1                       ///< 电机极对数(暂时没用到）
#define DEF_MOTOR_INC_PER_ROUND_CL_FENGDEKONG   4096                    ///< 电机转一圈位置增加总量
#define DEF_MOTOR_RPM_MAX_CL_FENGDEKONG         3000                    ///< 电机最大转速
#define DEF_MOTOR_MMPS_MAX_CL_FENGDEKONG        5000                    ///< 最大速度

//太兆
#define DEF_AUTO_ON_TIME_MS_CL_TAIZHAO       1500
#define DEF_WHEEL_DIAMETER_CL_TAIZHAO        170                  ///< 车轮直径/mm
#define DEF_WHEEL_SPACE_CL_TAIZHAO           530                  ///< 车轮间距/mm
#define DEF_MOTOR_DRIVE_TYPE_CL_TAIZHAO      MOTOR_DRIVE_TAIZHAO  ///< 电机驱动类型
#define DEF_MOTOR_ACC_UP_CL_TAIZHAO          500                  ///< 上升加速度 mm/s^2
#define DEF_MOTOR_ACC_DOWN_CL_TAIZHAO        2400                 ///< 下降加速度 mm/s^2
#define DEF_MOTOR_REDUCTION_RATIO_CL_TAIZHAO 10                   ///< 电机减速比 1
#define DEF_MOTOR_POLE_CL_TAIZHAO            1                    ///< 电机极对数(暂时没用到）
#define DEF_MOTOR_INC_PER_ROUND_CL_TAIZHAO   2048                 ///< 电机转一圈位置增加总量
#define DEF_MOTOR_RPM_MAX_CL_TAIZHAO         210                  ///< 电机最大转速
#define DEF_MOTOR_MMPS_MAX_CL_TAIZHAO        5000                 ///< 最大速度

#define EEPROM_ADDR       0XA0  ///< EEPROM 器件地址
#define SYS_PARAS_ADDR    0     ///< 系统配置参数存储地址
#define SYS_COREDUMP_ADDR 1024  ///< coredown 信息

#define DEF_ID                      1
#define DEF_NVG_RK_UP_TIMEOUT_MS    65000
#define DEF_NVG_RK_DOWN_TIMEOUT_MS  60000
#define DEF_TASK_RK_UP_TIMEOUT_MS   60000
#define DEF_TASK_RK_DOWN_TIMEOUT_MS 60000

#define DEF_LAMP_SHAKE_PERIOD   500
#define DEF_EMRRG_SHAKE_PERIOD  250
#define DEF_BUTTON_SHAKE_PERIOD 200

#define DEF_LEFT_LAMP_COLOR_R  OFF
#define DEF_LEFT_LAMP_COLOR_G  OFF
#define DEF_LEFT_LAMP_COLOR_B  OFF
#define DEF_RIGHT_LAMP_COLOR_R OFF
#define DEF_RIGHT_LAMP_COLOR_G OFF
#define DEF_RIGHT_LAMP_COLOR_B OFF

#define DEF_EPB_DELAY_MS 5000

#define DEF_IMU_PITCH_THRESHOLD 20
#define DEF_LUNA_DELT_THRESHOLD 10
#define DEF_LUNA_DIS_NORMAL     135
#define DEF_LUNA_DIS_MIN        25
#define DEF_LUNA_DIS_MAX        160
#define DEF_IMU_BUMP_SMP_INVLA  50
#define DEF_LUNA_SMP_INVLA      30
#define DEF_BUMP_UP_HOLD_TIME   500

#ifdef ROBOT_SECURITY
#define DEF_AUTO_ON_TIME_MS       DEF_AUTO_ON_TIME_MS_SC
#define DEF_WHEEL_DIAMETER        DEF_WHEEL_DIAMETER_SC
#define DEF_WHEEL_SPACE           DEF_WHEEL_SPACE_SC
#define DEF_MOTOR_DRIVE_TYPE      DEF_MOTOR_DRIVE_TYPE_SC
#define DEF_MOTOR_ACC_UP          DEF_MOTOR_ACC_UP_SC
#define DEF_MOTOR_ACC_DOWN        DEF_MOTOR_ACC_DOWN_SC
#define DEF_MOTOR_REDUCTION_RATIO DEF_MOTOR_REDUCTION_RATIO_SC
#define DEF_MOTOR_POLE            DEF_MOTOR_POLE_SC
#define DEF_MOTOR_INC_PER_ROUND   DEF_MOTOR_INC_PER_ROUND_SC
#define DEF_MOTOR_RPM_MAX         DEF_MOTOR_RPM_MAX_SC
#define DEF_MOTOR_MMPS_MAX        DEF_MOTOR_MMPS_MAX_SC

#elif defined BINGO

#ifdef USE_MOTOR_SELF

#define DEF_AUTO_ON_TIME_MS       DEF_AUTO_ON_TIME_MS_CL_SELF
#define DEF_WHEEL_DIAMETER        DEF_WHEEL_DIAMETER_CL_SELF
#define DEF_WHEEL_SPACE           DEF_WHEEL_SPACE_CL_SELF
#define DEF_MOTOR_DRIVE_TYPE      DEF_MOTOR_DRIVE_TYPE_CL_SELF
#define DEF_MOTOR_ACC_UP          DEF_MOTOR_ACC_UP_CL_SELF
#define DEF_MOTOR_ACC_DOWN        DEF_MOTOR_ACC_DOWN_CL_SELF
#define DEF_MOTOR_REDUCTION_RATIO DEF_MOTOR_REDUCTION_RATIO_CL_SELF
#define DEF_MOTOR_POLE            DEF_MOTOR_POLE_CL_SELF
#define DEF_MOTOR_INC_PER_ROUND   DEF_MOTOR_INC_PER_ROUND_CL_SELF
#define DEF_MOTOR_RPM_MAX         DEF_MOTOR_RPM_MAX_CL_SELF
#define DEF_MOTOR_MMPS_MAX        DEF_MOTOR_MMPS_MAX_CL_SELF

#elif defined USE_MOTOR_SYNTRON

#define DEF_AUTO_ON_TIME_MS       DEF_AUTO_ON_TIME_MS_CL
#define DEF_WHEEL_DIAMETER        DEF_WHEEL_DIAMETER_CL
#define DEF_WHEEL_SPACE           DEF_WHEEL_SPACE_CL
#define DEF_MOTOR_DRIVE_TYPE      DEF_MOTOR_DRIVE_TYPE_CL
#define DEF_MOTOR_ACC_UP          DEF_MOTOR_ACC_UP_CL
#define DEF_MOTOR_ACC_DOWN        DEF_MOTOR_ACC_DOWN_CL
#define DEF_MOTOR_REDUCTION_RATIO DEF_MOTOR_REDUCTION_RATIO_CL
#define DEF_MOTOR_POLE            DEF_MOTOR_POLE_CL
#define DEF_MOTOR_INC_PER_ROUND   DEF_MOTOR_INC_PER_ROUND_CL
#define DEF_MOTOR_RPM_MAX         DEF_MOTOR_RPM_MAX_CL
#define DEF_MOTOR_MMPS_MAX        DEF_MOTOR_MMPS_MAX_CL

#define DEF_MOTOR_CURRENT_SAMPLE_TIME        DEF_MOTOR_CURRENT_SAMPLE_TIME_CL
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX_CL
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN_CL
#define DEF_MOTOR_SYNTRON_CURRENT_WARN       DEF_MOTOR_SYNTRON_CURRENT_WARN_CL
#define DEF_MOTOR_SYNTRON_CURRENT_NORMAL     DEF_MOTOR_SYNTRON_CURRENT_NORMAL_CL

#elif defined USE_MOTOR_FENGDEKONG

#define DEF_AUTO_ON_TIME_MS       DEF_AUTO_ON_TIME_MS_CL_FENGDEKONG
#define DEF_WHEEL_DIAMETER        DEF_WHEEL_DIAMETER_CL_FENGDEKONG
#define DEF_WHEEL_SPACE           DEF_WHEEL_SPACE_CL_FENGDEKONG
#define DEF_MOTOR_DRIVE_TYPE      DEF_MOTOR_DRIVE_TYPE_CL_FENGDEKONG
#define DEF_MOTOR_ACC_UP          DEF_MOTOR_ACC_UP_CL_FENGDEKONG
#define DEF_MOTOR_ACC_DOWN        DEF_MOTOR_ACC_DOWN_CL_FENGDEKONG
#define DEF_MOTOR_REDUCTION_RATIO DEF_MOTOR_REDUCTION_RATIO_CL_FENGDEKONG
#define DEF_MOTOR_POLE            DEF_MOTOR_POLE_CL_FENGDEKONG
#define DEF_MOTOR_INC_PER_ROUND   DEF_MOTOR_INC_PER_ROUND_CL_FENGDEKONG
#define DEF_MOTOR_RPM_MAX         DEF_MOTOR_RPM_MAX_CL_FENGDEKONG
#define DEF_MOTOR_MMPS_MAX        DEF_MOTOR_MMPS_MAX_CL_FENGDEKONG

#elif defined USE_MOTOR_TAIZHAO

#define DEF_AUTO_ON_TIME_MS       DEF_AUTO_ON_TIME_MS_CL_TAIZHAO
#define DEF_WHEEL_DIAMETER        DEF_WHEEL_DIAMETER_CL_TAIZHAO
#define DEF_WHEEL_SPACE           DEF_WHEEL_SPACE_CL_TAIZHAO
#define DEF_MOTOR_DRIVE_TYPE      DEF_MOTOR_DRIVE_TYPE_CL_TAIZHAO
#define DEF_MOTOR_ACC_UP          DEF_MOTOR_ACC_UP_CL_TAIZHAO
#define DEF_MOTOR_ACC_DOWN        DEF_MOTOR_ACC_DOWN_CL_TAIZHAO
#define DEF_MOTOR_REDUCTION_RATIO DEF_MOTOR_REDUCTION_RATIO_CL_TAIZHAO
#define DEF_MOTOR_POLE            DEF_MOTOR_POLE_CL_TAIZHAO
#define DEF_MOTOR_INC_PER_ROUND   DEF_MOTOR_INC_PER_ROUND_CL_TAIZHAO
#define DEF_MOTOR_RPM_MAX         DEF_MOTOR_RPM_MAX_CL_TAIZHAO
#define DEF_MOTOR_MMPS_MAX        DEF_MOTOR_MMPS_MAX_CL_TAIZHAO

#define DEF_MOTOR_CURRENT_SAMPLE_TIME        DEF_MOTOR_CURRENT_SAMPLE_TIME_CL
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX_CL
#define DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN_CL
#define DEF_MOTOR_SYNTRON_CURRENT_WARN       DEF_MOTOR_SYNTRON_CURRENT_WARN_CL
#define DEF_MOTOR_SYNTRON_CURRENT_NORMAL     DEF_MOTOR_SYNTRON_CURRENT_NORMAL_CL

#endif

#endif

#define DEF_AUTO_CHARGE_LOC_UPDAT_MS 100

#define DEF_BATTERY_LOW_PROTECT 10

#define SYS_PARAS_SUCCESS 0
#define SYS_PARAS_FAILED  1

SYS_PARAS_T sys_paras      = {0};
SYS_PARAS_T sys_paras_temp = {0};

static struct fdb_blob blob;
extern struct fdb_kvdb kvdb;
/*****************************************************************/
/**
 * Function:       sys_paras_init
 * Description:    系统参数初始化
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_init(void) {
    uint16_t crc;

    if (sys_paras_read(&sys_paras) != SYS_PARAS_SUCCESS || sys_paras.motor_drive_type != DEF_MOTOR_DRIVE_TYPE) {
        sys_paras_write_def();
        sys_paras_read(&sys_paras);
    }

    crc = calculate_crc16((uint8_t *) &sys_paras, sizeof(sys_paras) - 2);

    log_i("Params SAVE CRC[0X%02X] WANT CRC[0X%02X]", sys_paras.crc, crc);

    if (sys_paras.crc != crc) {
        log_e("[ERROR] Robot selftest: Sys paras read.");
        sys_paras_write_def();

        sys_paras_read(&sys_paras);

        crc = calculate_crc16((uint8_t *) &sys_paras, sizeof(sys_paras) - 2);

        if (sys_paras.crc != crc) {
            log_e(
                "Write default paras to spiflash error, use code default "
                "paras");

            sys_exc_set(EXC49_NVG_EEPROM);
            sys_paras_use_def();
        } else {
            log_i("Write default paras to spiflash success.");

            sys_exc_clear(EXC49_NVG_EEPROM);
        }

    } else {
        log_i("[OK] Robot selftest: Sys paras read.");
    }

    return SYS_PARAS_SUCCESS;
}

/*****************************************************************/
/**
 * Function:       sys_paras_show
 * Description:    系统参数打印
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_show(void) {
    uint8_t  i;
    uint32_t on_time;

    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    sys_paras_read(&sys_paras_temp);

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    on_time = HAL_GetTick();

    printf("\r\n\r\n");
    printf(
        "***************************** MCU PARAMETERS SPIFLASH "
        "**********************\r\n\r\n");

    printf("Version                     : %s\r\n", SOFTWARE_VERSION);
    printf("Model Name                  : %s\r\n", MODEL_NAME);
    printf("Git Branch                  : %s\r\n", CURRENT_BRANCH);
    printf("Git Commit Id               : %s\r\n", COMMIT_ID);
    printf("Bulid Time                  : %s\r\n", BUILD_TIME);
    printf("Sys on time                 : %02ld:%02ld:%02ld\r\n", on_time / 1000 / 3600, on_time / 1000 % 3600 / 60,
           on_time / 1000 % 3600 % 60);
    printf("Sys current time            : %d-%d-%d %02d:%02d:%02d.%03ld\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
           time.Seconds, (time.SecondFraction - time.SubSeconds) * 4);
    printf("wwdg_timeout_coredown_cnt   : %ld \r\n", sys_paras_temp.wwdg_timeout_coredown_cnt);
    printf("hardfault_coredown_cnt      : %ld \r\n", sys_paras_temp.hardfault_coredown_cnt);
    printf("\r\n");

    printf("UID(96 bits)                : 0x%08lx%08lx%08lx \r\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
    printf("id                          : %ld \r\n", sys_paras_temp.id);
    printf("auto_on_time_ms             : %ld \r\n", sys_paras_temp.auto_on_time_ms);
    printf("nvg_rk_up_timeout_ms        : %ld \r\n", sys_paras_temp.nvg_rk_up_timeout_ms);
    printf("nvg_rk_down_timeout_ms      : %ld \r\n", sys_paras_temp.nvg_rk_down_timeout_ms);
    printf("task_rk_up_timeout_ms       : %ld \r\n", sys_paras_temp.task_rk_up_timeout_ms);
    printf("task_rk_down_timeout_ms     : %ld \r\n", sys_paras_temp.task_rk_down_timeout_ms);

    printf("left_color_r                : %ld \r\n", sys_paras_temp.left_color_r);
    printf("left_color_g                : %ld \r\n", sys_paras_temp.left_color_g);
    printf("left_color_b                : %ld \r\n", sys_paras_temp.left_color_b);
    printf("right_color_r               : %ld \r\n", sys_paras_temp.right_color_r);
    printf("right_color_g               : %ld \r\n", sys_paras_temp.left_color_g);
    printf("right_color_b               : %ld \r\n", sys_paras_temp.right_color_b);

    printf("epb_delay_ms                : %ld \r\n", sys_paras_temp.epb_delay_ms);

    printf("imu_pitch_threshold         : %ld (*0.1 C)\r\n", sys_paras_temp.imu_pitch_threshold);
    printf("luna_delt_threshold         : %ld cm\r\n", sys_paras_temp.luna_delt_threshold);
    printf("luna_dis_normal             : %ld cm\r\n", sys_paras_temp.luna_dis_normal);
    printf("luna_dis_min                : %ld cm\r\n", sys_paras_temp.luna_dis_min);
    printf("luna_dis_max                : %ld cm\r\n", sys_paras_temp.luna_dis_max);
    printf("imu_bump_smp_inval          : %ld ms\r\n", sys_paras_temp.imu_bump_smp_inval);
    printf("luna_smp_inval              : %ld ms\r\n", sys_paras_temp.luna_smp_inval);
    printf("bump_up_hold_time_ms        : %ld ms\r\n", sys_paras_temp.bump_up_hold_time_ms);

    printf("wheel_diameter              : %ld mm\r\n", sys_paras_temp.wheel_diameter);
    printf("wheel_space                 : %ld mm\r\n", sys_paras_temp.wheel_space);
    printf("motor_drive_type            : %ld \r\n", sys_paras_temp.motor_drive_type);
    printf("motor_acc_up                : %ld mm/s^2\r\n", sys_paras_temp.motor_acc_up);
    printf("motor_acc_down              : %ld mm/s^2\r\n", sys_paras_temp.motor_acc_down);
    printf("motor_reduction_ratio       : %ld \r\n", sys_paras_temp.motor_reduction_ratio);
    printf("motor_pole                  : %ld \r\n", sys_paras_temp.motor_pole);
    printf("motor_inc_per_round         : %ld \r\n", sys_paras_temp.motor_inc_per_round);
    printf("motor_rpm_max               : %ld \r\n", sys_paras_temp.motor_rpm_max);
    printf("motor_mmps_max              : %ld mm/s\r\n", sys_paras_temp.motor_mmps_max);
    printf("motor_max_current_output    : %ld A\r\n", sys_paras_temp.motor_current.motor_max_current_output);
    printf("motor_min_current_output    : %ld A\r\n", sys_paras_temp.motor_current.motor_min_current_output);
    printf("motor_warn_current          : %ld A\r\n", sys_paras_temp.motor_current.motor_warn_current);
    printf("motor_normal_current        : %ld A\r\n", sys_paras_temp.motor_current.motor_normal_current);
    printf("motor_current_sample_time   : %ld S\r\n", sys_paras_temp.motor_current.motor_current_sample_time);

    printf("auto_charge_loc_update_ms   : %ld \r\n", sys_paras_temp.auto_charge_loc_update_ms);
    printf("battery_low_protect         : %ld \r\n", sys_paras_temp.battery_low_protect);

    printf("sys_exc_detect_enable       : %ld \r\n", sys_paras_temp.sys_exc_detect_enable);
    printf("sys_exc_mask                : ");

    // printf("                              ");
    for (i = 1; i <= EXC_NUM; i++) {
        if (exc_get_item(sys_paras_temp.sys_exc_mask, i) == 0) {
            printf("\033[1;31;40m[%02d:%d]\033[0m ", i, exc_get_item(sys_paras_temp.sys_exc_mask, i));
        } else {
            printf("[%02d:%d] ", i, exc_get_item(sys_paras_temp.sys_exc_mask, i));
        }

        if (i % 16 == 0) {
            printf("\r\n");
            printf("                              ");
        }
    }

    printf("\r\n");

    printf(
        "\r\n\r\n**************************************************************"
        "*"
        "**********\r\n\r\n");

    return SYS_PARAS_SUCCESS;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys_paras_show, sys_paras_show, sys_paras_show);

/*****************************************************************/
/**
 * Function:       sys_paras_read
 * Description:    系统配置参数读取
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_read(SYS_PARAS_T *paras) {
    uint8_t  ret;
    uint32_t read_len;
    // eeprom_read((uint8_t*)paras, sizeof(SYS_PARAS_T),  EEPROM_ADDR,
    // SYS_PARAS_ADDR);
    read_len = fdb_kv_get_blob(&kvdb, "syspara", fdb_blob_make(&blob, (uint8_t *) paras, sizeof(SYS_PARAS_T)));
    // BL24C16_Read(SYS_PARAS_ADDR, (uint8_t*)paras, sizeof(SYS_PARAS_T));

    if (read_len != sizeof(SYS_PARAS_T)) {
        ret = SYS_PARAS_FAILED;
        log_e("[ERROR] Para read len[%d] parameters struct size[%d]", read_len, sizeof(SYS_PARAS_T));
    } else {
        ret = SYS_PARAS_SUCCESS;
    }

    return ret;
}

/*****************************************************************/
/**
 * Function:       sys_paras_write
 * Description:    系统配置参数写入
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_write(void) {
    sys_paras_temp.crc = calculate_crc16((uint8_t *) &sys_paras_temp, sizeof(sys_paras_temp) - 2);
    fdb_kv_set_blob(&kvdb, "syspara", fdb_blob_make(&blob, &sys_paras_temp, sizeof(sys_paras_temp)));

    // BL24C16_Write(SYS_PARAS_ADDR, (uint8_t*)&paras, sizeof(SYS_PARAS_T));
    return SYS_PARAS_SUCCESS;
}

/*****************************************************************/
/**
 * Function:       sys_paras_write_def
 * Description:    系统参数写默认参数
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_write_def(void) {
    sys_paras_temp.id                        = DEF_ID;
    sys_paras_temp.auto_on_time_ms           = DEF_AUTO_ON_TIME_MS;
    sys_paras_temp.wwdg_timeout_coredown_cnt = 0;
    sys_paras_temp.hardfault_coredown_cnt    = 0;
    sys_paras_temp.nvg_rk_up_timeout_ms      = DEF_NVG_RK_UP_TIMEOUT_MS;
    sys_paras_temp.nvg_rk_down_timeout_ms    = DEF_NVG_RK_DOWN_TIMEOUT_MS;
    sys_paras_temp.task_rk_up_timeout_ms     = DEF_TASK_RK_UP_TIMEOUT_MS;
    sys_paras_temp.task_rk_down_timeout_ms   = DEF_TASK_RK_DOWN_TIMEOUT_MS;

    sys_paras_temp.lamp_shake_period   = DEF_LAMP_SHAKE_PERIOD;
    sys_paras_temp.emerg_shake_period  = DEF_EMRRG_SHAKE_PERIOD;
    sys_paras_temp.button_shake_period = DEF_BUTTON_SHAKE_PERIOD;

    sys_paras_temp.left_color_r  = DEF_LEFT_LAMP_COLOR_R;
    sys_paras_temp.left_color_g  = DEF_LEFT_LAMP_COLOR_G;
    sys_paras_temp.left_color_b  = DEF_LEFT_LAMP_COLOR_B;
    sys_paras_temp.right_color_r = DEF_RIGHT_LAMP_COLOR_R;
    sys_paras_temp.right_color_g = DEF_RIGHT_LAMP_COLOR_G;
    sys_paras_temp.right_color_b = DEF_RIGHT_LAMP_COLOR_B;

    sys_paras_temp.epb_delay_ms = DEF_EPB_DELAY_MS;

    sys_paras_temp.imu_pitch_threshold  = DEF_IMU_PITCH_THRESHOLD;
    sys_paras_temp.luna_delt_threshold  = DEF_LUNA_DELT_THRESHOLD;
    sys_paras_temp.luna_dis_normal      = DEF_LUNA_DIS_NORMAL;
    sys_paras_temp.luna_dis_min         = DEF_LUNA_DIS_MIN;
    sys_paras_temp.luna_dis_max         = DEF_LUNA_DIS_MAX;
    sys_paras_temp.imu_bump_smp_inval   = DEF_IMU_BUMP_SMP_INVLA;
    sys_paras_temp.luna_smp_inval       = DEF_LUNA_SMP_INVLA;
    sys_paras_temp.bump_up_hold_time_ms = DEF_BUMP_UP_HOLD_TIME;

    sys_paras_temp.wheel_diameter                          = DEF_WHEEL_DIAMETER;
    sys_paras_temp.wheel_space                             = DEF_WHEEL_SPACE;
    sys_paras_temp.motor_drive_type                        = DEF_MOTOR_DRIVE_TYPE;
    sys_paras_temp.motor_acc_up                            = DEF_MOTOR_ACC_UP;
    sys_paras_temp.motor_acc_down                          = DEF_MOTOR_ACC_DOWN;
    sys_paras_temp.motor_reduction_ratio                   = DEF_MOTOR_REDUCTION_RATIO;
    sys_paras_temp.motor_pole                              = DEF_MOTOR_POLE;
    sys_paras_temp.motor_inc_per_round                     = DEF_MOTOR_INC_PER_ROUND;
    sys_paras_temp.motor_rpm_max                           = DEF_MOTOR_RPM_MAX;
    sys_paras_temp.motor_mmps_max                          = DEF_MOTOR_MMPS_MAX;
    sys_paras_temp.motor_current.motor_current_sample_time = DEF_MOTOR_CURRENT_SAMPLE_TIME;
    sys_paras_temp.motor_current.motor_max_current_output  = DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX;
    sys_paras_temp.motor_current.motor_min_current_output  = DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN;
    sys_paras_temp.motor_current.motor_normal_current      = DEF_MOTOR_SYNTRON_CURRENT_NORMAL;
    sys_paras_temp.motor_current.motor_warn_current        = DEF_MOTOR_SYNTRON_CURRENT_WARN;

    sys_paras_temp.auto_charge_loc_update_ms = DEF_AUTO_CHARGE_LOC_UPDAT_MS;

    sys_paras_temp.battery_low_protect = DEF_BATTERY_LOW_PROTECT;

    memset(sys_paras_temp.sys_exc_mask, 0XFF, sizeof(sys_paras_temp.sys_exc_mask));

    sys_paras_temp.sys_exc_detect_enable = 1;

#if defined BINGO
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC36_TASK_SECUR_TO);
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC66_CLIFF);
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC68_FLOOD);
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC69_RAIN);
#endif

#if defined ROBOT_SECURITY
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC15_MOTOR_ERR_FLAG);
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC51_NVG_TOUGH_EDGE);
    exc_clear_item(sys_paras_temp.sys_exc_mask, EXC69_RAIN);
#endif

    sys_paras_write();

    log_i("Write default paras to spiflash!");

    return SYS_PARAS_SUCCESS;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), set_def_paras, sys_paras_write_def, sys_paras_write_def);

/*****************************************************************/
/**
 * Function:       sys_paras_use_def
 * Description:    使用默认参数作为系统参数（SPIFLASH 异常时使用）
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_use_def(void) {
    sys_paras.id                        = DEF_ID;
    sys_paras.auto_on_time_ms           = DEF_AUTO_ON_TIME_MS;
    sys_paras.wwdg_timeout_coredown_cnt = 0;
    sys_paras.hardfault_coredown_cnt    = 0;
    sys_paras.nvg_rk_up_timeout_ms      = DEF_NVG_RK_UP_TIMEOUT_MS;
    sys_paras.nvg_rk_down_timeout_ms    = DEF_NVG_RK_DOWN_TIMEOUT_MS;
    sys_paras.task_rk_up_timeout_ms     = DEF_TASK_RK_UP_TIMEOUT_MS;
    sys_paras.task_rk_down_timeout_ms   = DEF_TASK_RK_DOWN_TIMEOUT_MS;

    sys_paras.lamp_shake_period   = DEF_LAMP_SHAKE_PERIOD;
    sys_paras.emerg_shake_period  = DEF_EMRRG_SHAKE_PERIOD;
    sys_paras.button_shake_period = DEF_BUTTON_SHAKE_PERIOD;

    sys_paras.left_color_r = DEF_LEFT_LAMP_COLOR_R;

    sys_paras.epb_delay_ms = DEF_EPB_DELAY_MS;

    sys_paras.imu_pitch_threshold  = DEF_IMU_PITCH_THRESHOLD;
    sys_paras.luna_delt_threshold  = DEF_LUNA_DELT_THRESHOLD;
    sys_paras.luna_dis_normal      = DEF_LUNA_DIS_NORMAL;
    sys_paras.luna_dis_min         = DEF_LUNA_DIS_MIN;
    sys_paras.luna_dis_max         = DEF_LUNA_DIS_MAX;
    sys_paras.imu_bump_smp_inval   = DEF_IMU_BUMP_SMP_INVLA;
    sys_paras.luna_smp_inval       = DEF_LUNA_SMP_INVLA;
    sys_paras.bump_up_hold_time_ms = DEF_BUMP_UP_HOLD_TIME;

    sys_paras.wheel_diameter                          = DEF_WHEEL_DIAMETER;
    sys_paras.wheel_space                             = DEF_WHEEL_SPACE;
    sys_paras.motor_drive_type                        = DEF_MOTOR_DRIVE_TYPE;
    sys_paras.motor_acc_up                            = DEF_MOTOR_ACC_UP;
    sys_paras.motor_acc_down                          = DEF_MOTOR_ACC_DOWN;
    sys_paras.motor_reduction_ratio                   = DEF_MOTOR_REDUCTION_RATIO;
    sys_paras.motor_pole                              = DEF_MOTOR_POLE;
    sys_paras.motor_inc_per_round                     = DEF_MOTOR_INC_PER_ROUND;
    sys_paras.motor_rpm_max                           = DEF_MOTOR_RPM_MAX;
    sys_paras.motor_mmps_max                          = DEF_MOTOR_MMPS_MAX;
    sys_paras.motor_current.motor_current_sample_time = DEF_MOTOR_CURRENT_SAMPLE_TIME;
    sys_paras.motor_current.motor_max_current_output  = DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MAX;
    sys_paras.motor_current.motor_min_current_output  = DEF_MOTOR_SYNTRON_OUTPUT_CURRENT_MIN;
    sys_paras.motor_current.motor_normal_current      = DEF_MOTOR_SYNTRON_CURRENT_NORMAL;
    sys_paras.motor_current.motor_warn_current        = DEF_MOTOR_SYNTRON_CURRENT_WARN;

    sys_paras.auto_charge_loc_update_ms = DEF_AUTO_CHARGE_LOC_UPDAT_MS;

    sys_paras.battery_low_protect = DEF_BATTERY_LOW_PROTECT;

    sys_paras.sys_exc_detect_enable = 1;
    memset(sys_paras.sys_exc_mask, 0XFF, sizeof(sys_paras.sys_exc_mask));

    exc_clear_item(sys_paras.sys_exc_mask, EXC35_NVG_SOC_COM_TO);
    exc_clear_item(sys_paras.sys_exc_mask, EXC66_CLIFF);
    exc_clear_item(sys_paras.sys_exc_mask, EXC69_RAIN);

    log_i("Use code default parameters!");

    return SYS_PARAS_SUCCESS;
}

/*****************************************************************/
/**
 * Function:       sys_paras_write_coredown
 * Description:    写入 coredown 信息
 * Calls:
 * Called By:
 * @param[in]      NONE
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_write_coredown(char *msg, uint32_t len) {
    // HAL_WWDG_Refresh(&hwwdg);
    // HAL_IWDG_Refresh(&hiwdg);
    fdb_kv_set_blob(&kvdb, "coredown", fdb_blob_make(&blob, (uint8_t *) msg, len));
    // HAL_WWDG_Refresh(&hwwdg);
    // HAL_IWDG_Refresh(&hiwdg);
    // BL24C16_Write(SYS_COREDUMP_ADDR, (uint8_t*)msg, write_len);

    return 0;
}

///< msg size should not smaller than 1024
uint8_t sys_paras_read_coredown(char *msg) {
    fdb_kv_get_blob(&kvdb, "coredown", fdb_blob_make(&blob, (uint8_t *) msg, 1024));
    // BL24C16_Read(SYS_COREDUMP_ADDR, (uint8_t*)msg, 1024);

    return 0;
}

uint8_t sys_paras_wwdg_to_inc(void) {
    sys_paras_read(&sys_paras_temp);
    sys_paras_temp.wwdg_timeout_coredown_cnt++;
    sys_paras_write();

    //< 更新到全局参数
    sys_paras.wwdg_timeout_coredown_cnt++;

    return SYS_PARAS_SUCCESS;
}

uint8_t sys_paras_hardfault_inc(void) {
    sys_paras_read(&sys_paras_temp);
    sys_paras_temp.hardfault_coredown_cnt++;
    sys_paras_write();

    //< 更新到全局参数
    sys_paras.hardfault_coredown_cnt++;

    return SYS_PARAS_SUCCESS;
}

/*****************************************************************/
/**
 * Function:       sys_paras_set
 * Description:    系统参数设置
 * Calls:
 * Called By:
 * @param[in]      item
 * @param[in]      val
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_set(PARAS_ITEM_E item, uint32_t val) {
    sys_paras_read(&sys_paras_temp);

    switch (item) {
        case SYS_PARAS_ID:
            sys_paras_temp.id = val;
            sys_paras.id      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_AUTO_ON_TIME:
            sys_paras_temp.auto_on_time_ms = val;
            sys_paras.auto_on_time_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_NRK_UP_TO:
            sys_paras_temp.nvg_rk_up_timeout_ms = val;
            sys_paras.nvg_rk_up_timeout_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_NRK_DOWN_TO:
            sys_paras_temp.nvg_rk_down_timeout_ms = val;
            sys_paras.nvg_rk_down_timeout_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_TRK_UP_TO:
            sys_paras_temp.task_rk_up_timeout_ms = val;
            sys_paras.task_rk_up_timeout_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_TRK_DOWN_TO:
            sys_paras_temp.task_rk_down_timeout_ms = val;
            sys_paras.task_rk_down_timeout_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_LAMP_PERIOD:
            sys_paras_temp.lamp_shake_period = val;
            sys_paras.lamp_shake_period      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_EMERG_PERIOD:
            sys_paras_temp.emerg_shake_period = val;
            sys_paras.emerg_shake_period      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_BUTTON_PERIOD:
            sys_paras_temp.button_shake_period = val;
            sys_paras.button_shake_period      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_LAMP_COLOR:
            sys_paras_temp.left_color_r  = val;
            sys_paras_temp.left_color_g  = val;
            sys_paras_temp.left_color_b  = val;
            sys_paras_temp.right_color_r = val;
            sys_paras_temp.right_color_g = val;
            sys_paras_temp.right_color_b = val;
            sys_paras.left_color_r       = val;  //< 更新到全局参数
            sys_paras.left_color_g       = val;
            sys_paras.left_color_b       = val;
            sys_paras.right_color_r      = val;
            sys_paras.right_color_g      = val;
            sys_paras.right_color_b      = val;
            break;

        case SYS_PARAS_EPB_DELAY:
            sys_paras_temp.epb_delay_ms = val;
            sys_paras.epb_delay_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_IMU_PITCH_H:
            sys_paras_temp.imu_pitch_threshold = val;
            sys_paras.imu_pitch_threshold      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_LUNA_DELT_H:
            sys_paras_temp.luna_delt_threshold = val;
            sys_paras.luna_delt_threshold      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_DIS_NOR:
            sys_paras_temp.luna_dis_normal = val;
            sys_paras.luna_dis_normal      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_DIS_MIN:
            sys_paras_temp.luna_dis_min = val;
            sys_paras.luna_dis_min      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_DIS_MAX:
            sys_paras_temp.luna_dis_max = val;
            sys_paras.luna_dis_max      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_IMU_BUMP_SMP_INV:
            sys_paras_temp.imu_bump_smp_inval = val;
            sys_paras.imu_bump_smp_inval      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_LUNA_SMP_INV:
            sys_paras_temp.luna_smp_inval = val;
            sys_paras.luna_smp_inval      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_BUMP_HOLD_TIME:
            sys_paras_temp.bump_up_hold_time_ms = val;
            sys_paras.bump_up_hold_time_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_DRIVE_TYPE:
            sys_paras_temp.motor_drive_type = val;
            sys_paras.motor_drive_type      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_ACC_UP:
            sys_paras_temp.motor_acc_up = val;
            sys_paras.motor_acc_up      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_WHEEL_DIAMETER:
            sys_paras_temp.wheel_diameter = val;
            sys_paras.wheel_diameter      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_WHEEL_SPACE:
            sys_paras_temp.wheel_space = val;
            sys_paras.wheel_space      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_ACC_DOWN:
            sys_paras_temp.motor_acc_down = val;
            sys_paras.motor_acc_down      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_REDUCTION_RATIO:
            sys_paras_temp.motor_reduction_ratio = val;
            sys_paras.motor_reduction_ratio      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_POLE:
            sys_paras_temp.motor_pole = val;
            sys_paras.motor_pole      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_INC_PER_ROUND:
            sys_paras_temp.motor_inc_per_round = val;
            sys_paras.motor_inc_per_round      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_RPM_MAX:
            sys_paras_temp.motor_rpm_max = val;
            sys_paras.motor_rpm_max      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_MOTOR_MMPS_MAX:
            sys_paras_temp.motor_mmps_max = val;
            sys_paras.motor_mmps_max      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_AUTO_CHARGE_POS_UPDATE_PERIOD:
            sys_paras_temp.auto_charge_loc_update_ms = val;
            sys_paras.auto_charge_loc_update_ms      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_BATTERT_LOW_PROTECT:
            sys_paras_temp.battery_low_protect = val;
            sys_paras.battery_low_protect      = val;  //< 更新到全局参数
            break;

        case SYS_PARAS_EXC_DETECT_ENABLE:
            sys_paras_temp.sys_exc_detect_enable = val;
            sys_paras.sys_exc_detect_enable      = val;  //< 更新到全局参数
            break;

        default:
            break;
    }

    sys_paras_write();

    return SYS_PARAS_SUCCESS;
}

/*****************************************************************/
/**
 * Function:       sys_paras_s
 * Description:    系统参数设置
 * Calls:
 * Called By:
 * @param[in]      item
 * @param[in]      val
 * @param[out]     NONE
 * @return         操作结果
 * 0 成功
 * 1 失败
 * @Others
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
uint8_t sys_paras_s(char *name, uint32_t val) {
    PARAS_ITEM_E item = SYS_PARAS_NULL;

    if (strcmp(name, "id") == 0) {
        item = SYS_PARAS_ID;
    } else if (strcmp(name, "auto_on_time_ms") == 0) {
        item = SYS_PARAS_AUTO_ON_TIME;
    } else if (strcmp(name, "nvg_rk_up_timeout_ms") == 0) {
        item = SYS_PARAS_NRK_UP_TO;
    } else if (strcmp(name, "nvg_rk_down_timeout_ms") == 0) {
        item = SYS_PARAS_NRK_DOWN_TO;
    } else if (strcmp(name, "task_rk_up_timeout_ms") == 0) {
        item = SYS_PARAS_TRK_UP_TO;
    } else if (strcmp(name, "task_rk_down_timeout_ms") == 0) {
        item = SYS_PARAS_TRK_DOWN_TO;
    } else if (strcmp(name, "lamp_period") == 0) {
        item = SYS_PARAS_LAMP_PERIOD;
    } else if (strcmp(name, "emerg_period") == 0) {
        item = SYS_PARAS_EMERG_PERIOD;
    } else if (strcmp(name, "button_period") == 0) {
        item = SYS_PARAS_BUTTON_PERIOD;
    } else if (strcmp(name, "lamp_color") == 0) {
        item = SYS_PARAS_LAMP_COLOR;
    } else if (strcmp(name, "epb_delay_ms") == 0) {
        item = SYS_PARAS_EPB_DELAY;
    } else if (strcmp(name, "imu_pitch_threshold") == 0) {
        item = SYS_PARAS_IMU_PITCH_H;
    } else if (strcmp(name, "luna_delt_threshold") == 0) {
        item = SYS_PARAS_LUNA_DELT_H;
    } else if (strcmp(name, "luna_dis_normal") == 0) {
        item = SYS_PARAS_DIS_NOR;
    } else if (strcmp(name, "luna_dis_min") == 0) {
        item = SYS_PARAS_DIS_MIN;
    } else if (strcmp(name, "luna_dis_max") == 0) {
        item = SYS_PARAS_DIS_MAX;
    } else if (strcmp(name, "imu_bump_smp_inval") == 0) {
        item = SYS_PARAS_IMU_BUMP_SMP_INV;
    } else if (strcmp(name, "luna_smp_inval") == 0) {
        item = SYS_PARAS_LUNA_SMP_INV;
    } else if (strcmp(name, "bump_up_hold_time_ms") == 0) {
        item = SYS_PARAS_BUMP_HOLD_TIME;
    } else if (strcmp(name, "wheel_diameter") == 0) {
        item = SYS_PARAS_WHEEL_DIAMETER;
    } else if (strcmp(name, "wheel_space") == 0) {
        item = SYS_PARAS_WHEEL_SPACE;
    } else if (strcmp(name, "motor_drive_type") == 0) {
        item = SYS_PARAS_MOTOR_DRIVE_TYPE;
    } else if (strcmp(name, "motor_acc_up") == 0) {
        item = SYS_PARAS_MOTOR_ACC_UP;
    } else if (strcmp(name, "motor_acc_down") == 0) {
        item = SYS_PARAS_MOTOR_ACC_DOWN;
    } else if (strcmp(name, "motor_reduction_ratio") == 0) {
        item = SYS_PARAS_MOTOR_REDUCTION_RATIO;
    } else if (strcmp(name, "motor_pole") == 0) {
        item = SYS_PARAS_MOTOR_POLE;
    } else if (strcmp(name, "motor_inc_per_round") == 0) {
        item = SYS_PARAS_MOTOR_INC_PER_ROUND;
    } else if (strcmp(name, "auto_charge_loc_update_ms") == 0) {
        item = SYS_PARAS_AUTO_CHARGE_POS_UPDATE_PERIOD;
    } else if (strcmp(name, "battery_low_protect") == 0) {
        item = SYS_PARAS_BATTERT_LOW_PROTECT;
    } else if (strcmp(name, "sys_exc_detect_enable") == 0) {
        item = SYS_PARAS_EXC_DETECT_ENABLE;
    } else {
        log_e("sys parameter not found!");
        return SYS_PARAS_FAILED;
    }

    sys_paras_set(item, val);
    log_i("sys parameter save!");
    return SYS_PARAS_SUCCESS;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), setParas, sys_paras_s, set sys parameter);

uint8_t sys_paras_motor_drive_def(char *type) {
    sys_paras_read(&sys_paras_temp);

    if (strcmp(type, "self") == 0) {
        sys_paras_temp.wheel_diameter        = DEF_WHEEL_DIAMETER_SC;
        sys_paras_temp.wheel_space           = DEF_WHEEL_SPACE_SC;
        sys_paras_temp.motor_drive_type      = DEF_MOTOR_DRIVE_TYPE_SC;
        sys_paras_temp.motor_acc_up          = DEF_MOTOR_ACC_UP_SC;
        sys_paras_temp.motor_acc_down        = DEF_MOTOR_ACC_DOWN_SC;
        sys_paras_temp.motor_reduction_ratio = DEF_MOTOR_REDUCTION_RATIO_SC;
        sys_paras_temp.motor_pole            = DEF_MOTOR_POLE_SC;
        sys_paras_temp.motor_inc_per_round   = DEF_MOTOR_INC_PER_ROUND_SC;
        sys_paras_temp.motor_rpm_max         = DEF_MOTOR_RPM_MAX_SC;
        sys_paras_temp.motor_mmps_max        = DEF_MOTOR_MMPS_MAX_SC;
    } else if (strcmp(type, "syntron") == 0) {
        sys_paras_temp.wheel_diameter        = DEF_WHEEL_DIAMETER_CL;
        sys_paras_temp.wheel_space           = DEF_WHEEL_SPACE_CL;
        sys_paras_temp.motor_drive_type      = DEF_MOTOR_DRIVE_TYPE_CL;
        sys_paras_temp.motor_acc_up          = DEF_MOTOR_ACC_UP_CL;
        sys_paras_temp.motor_acc_down        = DEF_MOTOR_ACC_DOWN_CL;
        sys_paras_temp.motor_reduction_ratio = DEF_MOTOR_REDUCTION_RATIO_CL;
        sys_paras_temp.motor_pole            = DEF_MOTOR_POLE_CL;
        sys_paras_temp.motor_inc_per_round   = DEF_MOTOR_INC_PER_ROUND_CL;
        sys_paras_temp.motor_rpm_max         = DEF_MOTOR_RPM_MAX_CL;
        sys_paras_temp.motor_mmps_max        = DEF_MOTOR_MMPS_MAX_CL;
    } else {
        printf("motor drive type not found.\r\n");

        return 1;
    }

    sys_paras_write();

    printf("motor drive set success, need to reboot.\r\n");

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motor_drive_def, sys_paras_motor_drive_def,
                 sys_paras_motor_drive_def);

uint8_t sys_paras_write_exc_mask(uint8_t item, uint8_t is_set) {
    sys_paras_read(&sys_paras_temp);

    if (is_set) {
        exc_set_item(sys_paras_temp.sys_exc_mask, item);
        exc_set_item(sys_paras.sys_exc_mask, item);  //< 实时生效
    } else {
        exc_clear_item(sys_paras_temp.sys_exc_mask, item);
        exc_clear_item(sys_paras.sys_exc_mask, item);  //< 实时生效
        ///< 把对应已触发的系统异常清除
        sys_exc_clear(item);
    }

    sys_paras_write();

    printf("save paras ok.\r\n");

    return SYS_PARAS_SUCCESS;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys_exc_mask, sys_paras_write_exc_mask,
                 sys_paras_write_exc_mask);
