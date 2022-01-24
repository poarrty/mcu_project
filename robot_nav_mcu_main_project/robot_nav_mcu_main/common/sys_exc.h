#ifndef _SYS_EXC_H
#define _SYS_EXC_H
#include "stdint.h"
#include "stdbool.h"

#define SYS_SET_BIT(num, bit) ((num) |= (1 << bit))
#define SYS_GET_BIT(num, bit) ((num >> bit) & 1)
#define SYS_CLR_BIT(num, bit) ((num) &= (~(1 << bit)))

#define EXC_NUM 80  ///< 需要是8的整数倍
typedef enum {
    EXC1_MOTOR1L_ELE_OVFLOW = 1,
    EXC1_MOTOR1R_ELE_OVFLOW = 2,
    EXC05_MOTOR1L_LOCK      = 5,
    EXC06_MOTOR1R_LOCK      = 6,
    EXC09_MOTOR1L_LRT       = 9,   ///< 左电机堵转
    EXC10_MOTOR1R_LRT       = 10,  ///< 右电机堵转
    EXC15_MOTOR_ERR_FLAG    = 15,  ///< 驱动器错误总标志
    EXC33_MOTOR_COM_TO      = 33,
    EXC34_BATT_TO           = 34,
    EXC35_NVG_SOC_COM_TO    = 35,
    EXC36_TASK_SECUR_TO     = 36,
    EXC39_IMU_DATA          = 39,
    EXC44_GPS_TO            = 44,
    EXC49_NVG_EEPROM        = 49,
    EXC50_NVG_IR            = 50,
    EXC51_NVG_TOUGH_EDGE    = 51,
    EXC52_MCU_REBOOT        = 52,
    EXC65_EMERG             = 65,
    EXC66_CLIFF             = 66,
    EXC67_CRASH             = 67,
    EXC68_FLOOD             = 68,
    EXC69_RAIN              = 69,
    EXC70_LOW_POWER         = 70

} SYS_EXC_E;

#pragma pack(1)

typedef struct {
    uint8_t  id;
    uint8_t  val;
    uint32_t update_ts;
    uint32_t update_thread;
    uint32_t set_cnt;
} EXC_ITEM_T, *EXC_ITEM_P;

#pragma pack()

void sys_exc_init(void);
void sys_exc_set(uint8_t item);
void sys_exc_clear(uint8_t item);
void sys_exc_clear_all(void);
bool sys_exc_get(uint8_t item);
bool is_sys_exc_none(void);
bool is_sys_danger_exc_detected(void);

bool exc_get_item(uint8_t *exc, uint8_t item);
void exc_set_item(uint8_t *exc, uint8_t item);
void exc_clear_item(uint8_t *exc, uint8_t item);

void     sys_exc_groud_set(uint8_t index, uint16_t value);
uint16_t sys_exc_groud_get(uint8_t index);

#endif
