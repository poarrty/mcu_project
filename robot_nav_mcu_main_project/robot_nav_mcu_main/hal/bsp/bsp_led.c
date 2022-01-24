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

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_led.h"
#include "gpio.h"
#include "shell.h"
#include "string.h"
#include "stdio.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_LED  - LED
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
#define LED_PWR_BUTTON_GREEN_GPIO_Port GPIOC
#define LED_PWR_BUTTON_GREEN_Pin       GPIO_PIN_9

//#define ROBOT_42

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
 * Function:       bsp_led_init
 * Description:    初始化 bsp_led
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
int bsp_led_init(void) {
    /*添加模块处理函数*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();

    HAL_GPIO_WritePin(LED_PWR_BUTTON_GREEN_GPIO_Port, LED_PWR_BUTTON_GREEN_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = LED_PWR_BUTTON_GREEN_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PWR_BUTTON_GREEN_GPIO_Port, &GPIO_InitStruct);

#if defined ROBOT_42
    HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_SET);
#endif

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_led_deInit
 * Description:    释放 bsp_led 资源
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
int bsp_led_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_led_on
 * Description:    点亮 LED
 * Calls:
 * Called By:
 * @param[in]      led 指定led
 * @param[out]     NONE
 * @return
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
void bsp_led_on(LED_E led) {
    switch (led) {
        case LED_POWER_BUTTON:
#if defined ROBOT_42
            HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_RESET);
#else
            HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_RESET);
#endif
            HAL_GPIO_WritePin(LED_PWR_BUTTON_GREEN_GPIO_Port, LED_PWR_BUTTON_GREEN_Pin, GPIO_PIN_SET);
            break;

        case LED_DRV:
            HAL_GPIO_WritePin(MX_LED_DRIVE_GPIO_Port, MX_LED_DRIVE_Pin, GPIO_PIN_SET);
            break;

        case LED_EMERG:
            // HAL_GPIO_WritePin(MX_LED_EMERG_GPIO_Port, MX_LED_EMERG_Pin, GPIO_PIN_RESET);
            break;

        case LED_RUN:
            HAL_GPIO_WritePin(MX_LED_RUN_GPIO_Port, MX_LED_RUN_Pin, GPIO_PIN_SET);
            break;

        case LED_LEFT_R:
            HAL_GPIO_WritePin(MX_LED_LEFT_R_GPIO_Port, MX_LED_LEFT_R_Pin, GPIO_PIN_SET);
            break;

        case LED_LEFT_G:
            HAL_GPIO_WritePin(MX_LED_LEFT_G_GPIO_Port, MX_LED_LEFT_G_Pin, GPIO_PIN_SET);
            break;

        case LED_LEFT_B:
            HAL_GPIO_WritePin(MX_LED_LEFT_B_GPIO_Port, MX_LED_LEFT_B_Pin, GPIO_PIN_SET);
            break;

        case LED_RIGHT_R:
            HAL_GPIO_WritePin(MX_LED_RIGHT_R_GPIO_Port, MX_LED_RIGHT_R_Pin, GPIO_PIN_SET);
            break;

        case LED_RIGHT_G:
            HAL_GPIO_WritePin(MX_LED_RIGHT_G_GPIO_Port, MX_LED_RIGHT_G_Pin, GPIO_PIN_SET);
            break;

        case LED_RIGHT_B:
            HAL_GPIO_WritePin(MX_LED_RIGHT_B_GPIO_Port, MX_LED_RIGHT_B_Pin, GPIO_PIN_SET);
            break;

        default:
            break;
    }
}

/*****************************************************************/
/**
 * Function:       bsp_led_on
 * Description:    熄灭 LED
 * Calls:
 * Called By:
 * @param[in]      led 指定led
 * @param[out]     NONE
 * @return         NONE
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
void bsp_led_off(LED_E led) {
    switch (led) {
        case LED_POWER_BUTTON:
#if defined ROBOT_42
            HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_SET);
#else
            HAL_GPIO_WritePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin, GPIO_PIN_SET);
#endif
            HAL_GPIO_WritePin(LED_PWR_BUTTON_GREEN_GPIO_Port, LED_PWR_BUTTON_GREEN_Pin, GPIO_PIN_RESET);
            break;

        case LED_DRV:
            HAL_GPIO_WritePin(MX_LED_DRIVE_GPIO_Port, MX_LED_DRIVE_Pin, GPIO_PIN_RESET);
            break;

        case LED_EMERG:
            // HAL_GPIO_WritePin(MX_LED_EMERG_GPIO_Port, MX_LED_EMERG_Pin, GPIO_PIN_SET);
            break;

        case LED_RUN:
            HAL_GPIO_WritePin(MX_LED_RUN_GPIO_Port, MX_LED_RUN_Pin, GPIO_PIN_RESET);
            break;

        case LED_LEFT_R:
            HAL_GPIO_WritePin(MX_LED_LEFT_R_GPIO_Port, MX_LED_LEFT_R_Pin, GPIO_PIN_RESET);
            break;

        case LED_LEFT_G:
            HAL_GPIO_WritePin(MX_LED_LEFT_G_GPIO_Port, MX_LED_LEFT_G_Pin, GPIO_PIN_RESET);
            break;

        case LED_LEFT_B:
            HAL_GPIO_WritePin(MX_LED_LEFT_B_GPIO_Port, MX_LED_LEFT_B_Pin, GPIO_PIN_RESET);
            break;

        case LED_RIGHT_R:
            HAL_GPIO_WritePin(MX_LED_RIGHT_R_GPIO_Port, MX_LED_RIGHT_R_Pin, GPIO_PIN_RESET);
            break;

        case LED_RIGHT_G:
            HAL_GPIO_WritePin(MX_LED_RIGHT_G_GPIO_Port, MX_LED_RIGHT_G_Pin, GPIO_PIN_RESET);
            break;

        case LED_RIGHT_B:
            HAL_GPIO_WritePin(MX_LED_RIGHT_B_GPIO_Port, MX_LED_RIGHT_B_Pin, GPIO_PIN_RESET);
            break;

        default:
            break;
    }
}

/*****************************************************************/
/**
 * Function:       bsp_led_on
 * Description:    翻转 LED
 * Calls:
 * Called By:
 * @param[in]      led 指定led
 * @param[out]     NONE
 * @return         NONE
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
void bsp_led_toggle(LED_E led) {
    switch (led) {
        case LED_POWER_BUTTON:
            HAL_GPIO_TogglePin(MX_LED_PWR_BUTTON_GPIO_Port, MX_LED_PWR_BUTTON_Pin);
            HAL_GPIO_TogglePin(LED_PWR_BUTTON_GREEN_GPIO_Port, LED_PWR_BUTTON_GREEN_Pin);
            break;

        case LED_LEFT_R:
            HAL_GPIO_TogglePin(MX_LED_LEFT_R_GPIO_Port, MX_LED_LEFT_R_Pin);
            break;

        case LED_DRV:
            HAL_GPIO_TogglePin(MX_LED_DRIVE_GPIO_Port, MX_LED_DRIVE_Pin);
            break;

        case LED_EMERG:
            // HAL_GPIO_TogglePin(MX_LED_EMERG_GPIO_Port, MX_LED_EMERG_Pin);
            break;

        case LED_RUN:
            HAL_GPIO_TogglePin(MX_LED_RUN_GPIO_Port, MX_LED_RUN_Pin);
            break;

        case LED_LEFT_G:
            HAL_GPIO_TogglePin(MX_LED_LEFT_G_GPIO_Port, MX_LED_LEFT_G_Pin);
            break;

        case LED_LEFT_B:
            HAL_GPIO_TogglePin(MX_LED_LEFT_B_GPIO_Port, MX_LED_LEFT_B_Pin);
            break;

        case LED_RIGHT_R:
            HAL_GPIO_TogglePin(MX_LED_RIGHT_R_GPIO_Port, MX_LED_RIGHT_R_Pin);
            break;

        case LED_RIGHT_G:
            HAL_GPIO_TogglePin(MX_LED_RIGHT_G_GPIO_Port, MX_LED_RIGHT_G_Pin);
            break;

        case LED_RIGHT_B:
            HAL_GPIO_TogglePin(MX_LED_RIGHT_B_GPIO_Port, MX_LED_RIGHT_B_Pin);
            break;

        default:
            break;
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_led_toggle, bsp_led_toggle, bsp_led_toggle);

/*****************************************************************/
/**
 * Function:       bsp_lamp_color
 * Description:    设置示廓灯颜色
 * Calls:
 * Called By:
 * @param[in]      lamp 指定左右侧示廓灯
 * @param[in]      color 指定颜色
 * @param[out]     NONE
 * @return         NONE
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
void bsp_lamp_color(LAMP_E lamp, LED_COLOR_E color) {
    if (lamp == LAMP_Left) {
        switch (color) {
            case COLOR_OFF:
                bsp_led_off(LED_LEFT_R);
                bsp_led_off(LED_LEFT_G);
                bsp_led_off(LED_LEFT_B);
                break;

            case COLOR_RED:
                bsp_led_on(LED_LEFT_R);
                bsp_led_off(LED_LEFT_G);
                bsp_led_off(LED_LEFT_B);
                break;

            case COLOR_GREEN:
                bsp_led_off(LED_LEFT_R);
                bsp_led_on(LED_LEFT_G);
                bsp_led_off(LED_LEFT_B);
                break;

            case COLOR_BLUE:
                bsp_led_off(LED_LEFT_R);
                bsp_led_off(LED_LEFT_G);
                bsp_led_on(LED_LEFT_B);
                break;

            case COLOR_WHITE:
                bsp_led_on(LED_LEFT_R);
                bsp_led_on(LED_LEFT_G);
                bsp_led_on(LED_LEFT_B);
                break;

            case COLOR_PURPLE:
                bsp_led_on(LED_LEFT_R);
                bsp_led_off(LED_LEFT_G);
                bsp_led_on(LED_LEFT_B);
                break;

            case COLOR_YELLOW:
                bsp_led_on(LED_LEFT_R);
                bsp_led_on(LED_LEFT_G);
                bsp_led_off(LED_LEFT_B);
                break;

            case COLOR_CYAN:
                bsp_led_off(LED_LEFT_R);
                bsp_led_on(LED_LEFT_G);
                bsp_led_on(LED_LEFT_B);
                break;

            default:
                break;
        }
    } else if (lamp == LAMP_Right) {
        switch (color) {
            case COLOR_OFF:
                bsp_led_off(LED_RIGHT_R);
                bsp_led_off(LED_RIGHT_G);
                bsp_led_off(LED_RIGHT_B);
                break;

            case COLOR_RED:
                bsp_led_on(LED_RIGHT_R);
                bsp_led_off(LED_RIGHT_G);
                bsp_led_off(LED_RIGHT_B);
                break;

            case COLOR_GREEN:
                bsp_led_off(LED_RIGHT_R);
                bsp_led_on(LED_RIGHT_G);
                bsp_led_off(LED_RIGHT_B);
                break;

            case COLOR_BLUE:
                bsp_led_off(LED_RIGHT_R);
                bsp_led_off(LED_RIGHT_G);
                bsp_led_on(LED_RIGHT_B);
                break;

            case COLOR_WHITE:
                bsp_led_on(LED_RIGHT_R);
                bsp_led_on(LED_RIGHT_G);
                bsp_led_on(LED_RIGHT_B);
                break;

            case COLOR_PURPLE:
                bsp_led_on(LED_RIGHT_R);
                bsp_led_off(LED_RIGHT_G);
                bsp_led_on(LED_RIGHT_B);
                break;

            case COLOR_YELLOW:
                bsp_led_on(LED_RIGHT_R);
                bsp_led_on(LED_RIGHT_G);
                bsp_led_off(LED_RIGHT_B);
                break;

            case COLOR_CYAN:
                bsp_led_off(LED_RIGHT_R);
                bsp_led_on(LED_RIGHT_G);
                bsp_led_on(LED_RIGHT_B);
                break;

            default:
                break;
        }
    }
}

uint8_t lampColor(char *lamp, char *color) {
    LAMP_E      lamp_e  = {0};
    LED_COLOR_E color_e = {0};

    if (strcmp("l", lamp) == 0) {
        lamp_e = LAMP_Left;
    } else if (strcmp("r", lamp) == 0) {
        lamp_e = LAMP_Right;
    } else {
        printf("first parameter error! l or r");
    }

    if (strcmp("red", color) == 0) {
        color_e = COLOR_RED;
    } else if (strcmp("green", color) == 0) {
        color_e = COLOR_GREEN;
    } else if (strcmp("blue", color) == 0) {
        color_e = COLOR_BLUE;
    } else if (strcmp("while", color) == 0) {
        color_e = COLOR_WHITE;
    } else if (strcmp("purple", color) == 0) {
        color_e = COLOR_PURPLE;
    } else if (strcmp("yellow", color) == 0) {
        color_e = COLOR_YELLOW;
    } else if (strcmp("cyan", color) == 0) {
        color_e = COLOR_CYAN;
    } else {
        printf("second parameter error! red green blue while puple yellow cyan");
    }

    bsp_lamp_color(lamp_e, color_e);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), lampColor, lampColor, set lamp color);

#ifdef __cplusplus
}
#endif

/* @} BSP_LED */
/* @} Robot_BSP */
