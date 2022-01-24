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
#include "bsp_button.h"
#define LOG_TAG "bsp_button"
#include "elog.h"
#include "sys_pubsub.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_BUTTON 板级按键处理 - bsp_button
 *
 * @brief bsp_button 负责控制处理
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
struct Button button_pwr;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static uint8_t read_button_pwr(void);
static void    button_pwr_sclick_handler(void *btn);
static void    button_pwr_dclick_handler(void *btn);
static void    button_pwr_lpress_handler(void *btn);
/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_button_init
 * Description:    初始化 bsp_button
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示打开文件成功
 *  - 1 表示打开文件失败
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
int bsp_button_init(void) {
    /*添加模块处理函数*/
    button_init(&button_pwr, read_button_pwr, GPIO_PIN_RESET);
    button_attach(&button_pwr, SINGLE_CLICK, button_pwr_sclick_handler);
    button_attach(&button_pwr, DOUBLE_CLICK, button_pwr_dclick_handler);
    button_attach(&button_pwr, LONG_PRESS_START, button_pwr_lpress_handler);
    button_start(&button_pwr);

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_button_deInit
 * Description:    释放 bsp_button 资源
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
int bsp_button_deInit(void) {
    return 0;
}

uint8_t read_button_pwr(void) {
    return HAL_GPIO_ReadPin(MX_BUTTON_PWR_GPIO_Port, MX_BUTTON_PWR_Pin);
}

static void button_pwr_sclick_handler(void *btn) {
    log_d("Power button single click!");

    BUTTON_STA_T button_sta;

    button_sta.button = BUTTON_PWR;
    button_sta.event  = SINGLE_CLICK;

    xQueueOverwrite(topic_button, &button_sta);

    osEventFlagsSet(sys_pub, SYS_EVT_BUTTON_STA);
}

static void button_pwr_dclick_handler(void *btn) {}

static void button_pwr_lpress_handler(void *btn) {
    log_d("Power button long press start!");

    bsp_led_toggle(LED_POWER_BUTTON);

    while (HAL_GPIO_ReadPin(MX_BUTTON_PWR_GPIO_Port, MX_BUTTON_PWR_Pin) == GPIO_PIN_RESET)
        ;

    HAL_NVIC_SystemReset();
}

#ifdef __cplusplus
}
#endif

/* @} BSP_BUTTON */
/* @} Robot_BSP */
