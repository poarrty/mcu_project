/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name:
 * @Author: (linzhongxing@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 19:05:00
 * @Description:
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BSP_CLEAN_FUN_H__
#define __BSP_CLEAN_FUN_H__

#include <stdint.h>

#define MAX_PUMP_PWM_VALUE 1000 - 1             // pwm最大值
#define MAX_CLEANER_DIAPHRAGM_PUMP_TIMEOUT 120  //清洁剂隔膜泵超时
#define MAX_TRANSITION_BOX_CLEAN_TIMEOUT 30     //过渡箱清洗超时

typedef enum {
    ADD_WATER_IDLE = 0,
    TRANSITION_BOX_CLEAN,
    ADD_WATER,
    SEWAGE_PUMP,
    ADD_WATER_PUMP_START_FLAG,
    SEWAGE_PUMP_START_FLAG,

} add_water_status_enum_t;

typedef enum {
    PUMP_OFF = 0x00,
    PUMP_ON = 0x01,

} pump_status_enum_t;

typedef enum {
    WATERING,
    DRAINING,
    CLEANING,
    STOPING,

} workstation_state_enum_t;

typedef struct {
    workstation_state_enum_t workstation_state;
    uint8_t flag;
} workstation_state_stu_t;

typedef struct {
    pump_status_enum_t sewage_pump_status;     //排污泵状态
    pump_status_enum_t add_water_pump_status;  //加水泵状态
    uint16_t sewage_pump_pwm_pulse;            //排污泵高电平宽度
    uint16_t add_water_pump_pwm_pulse;         //加水泵高电平宽度
} pump_pwm_stu_t;

typedef struct {
    //排污水超时
    uint16_t sewage_timeout;
    //清洁剂隔膜泵超时
    uint16_t cleaner_diaphragm_pump_timeout;
    //过渡箱清洗超时
    uint16_t transition_box_clean_timeout;
    add_water_status_enum_t add_water_status;

} add_water_var_stu_t;

extern add_water_var_stu_t add_water_var;
extern pump_pwm_stu_t pump_pwm;

void set_cleaner_diaphragm_pump_timeout(uint16_t value);
void set_transition_box_clean_timeout(uint16_t value);
void add_water_pump_start(void);
void add_water_pump_stop(void);
void sewage_pump_stop(void);
void sewage_pump_puase(void);
void sewage_pump_start(void);
void all_device_start(void);
void all_clean_fun_stop(void);

#endif
