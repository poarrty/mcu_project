/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/
#ifndef _BSP_LED_H
#define _BSP_LED_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup BSP_LED
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef enum {
    LED_POWER_BUTTON,
    LED_EMERG_BUTTON,
    LED_DRV,
    LED_EMERG,
    LED_RUN,
    LED_LEFT_R,
    LED_LEFT_G,
    LED_LEFT_B,
    LED_RIGHT_R,
    LED_RIGHT_G,
    LED_RIGHT_B

} LED_E;

typedef enum {
    COLOR_OFF    = 0,
    COLOR_RED    = 0x01 << 1,
    COLOR_GREEN  = 0x01 << 2,
    COLOR_BLUE   = 0x01 << 3,
    COLOR_WHITE  = 0x01 << 4,
    COLOR_PURPLE = 0x01 << 5,
    COLOR_YELLOW = 0x01 << 6,
    COLOR_CYAN   = 0x01 << 7,

} LED_COLOR_E;

typedef enum {
    LAMP_Center,
    LAMP_Left,
    LAMP_Right,
    LAMP_Up,
    LAMP_Down,
    LAMP_LeftUp,
    LAMP_LeftDown,
    LAMP_RightUp,
    LAMP_RightDown

} LAMP_E;

typedef enum {
    color_R,
    color_G,
    color_B,

} COLOR_RGB;

typedef enum {
    OFF,
    ON,

} COLOR_CTRL;  //仅针对此项目，只区分0/1，实际灯光范围为0～255
/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void bsp_led_on(LED_E led);
void bsp_led_off(LED_E led);
void bsp_led_toggle(LED_E led);
void bsp_lamp_color(LAMP_E lamp, LED_COLOR_E color);

/*****************************************************************
功  能: 初始化 bsp_led
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_led_init(void);

/*****************************************************************
功  能: 释放 bsp_led 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_led_deInit(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} BSP_LED */

#endif
