/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-21
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_pmu.h"
#include "stdint.h"
#include "gpio.h"
#include "shell.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_PMU - BSP_PMU
 *
 * @brief 电源管理接口\n
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
 * Function:       bsp_pmu_init
 * Description:    初始化 bsp_pmu
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
int bsp_pmu_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_pmu_deInit
 * Description:    释放 bsp_pmu 资源
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
int bsp_pmu_deInit(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_pmu_all_on
 * Description:    打开所有设备电源
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
uint8_t bsp_pmu_all_on(void) {
    HAL_GPIO_WritePin(MX_PWR_SOC_GPIO_Port, MX_PWR_SOC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_SET);
    ///< MCU CAN1 初始化前此电源要先上电，否则初始化失败
    // HAL_GPIO_WritePin(MX_PWR_MISC1_GPIO_Port, MX_PWR_MISC1_Pin,
    // GPIO_PIN_RESET);    ///<
    // 任务模块485、IMU、GPS、板内温湿度传感器、电池&驱动器CAN收发芯片
    HAL_GPIO_WritePin(MX_PWR_MISC2_GPIO_Port, MX_PWR_MISC2_Pin,
                      GPIO_PIN_SET);  ///< 安全触边、水浸和红外接收
    HAL_GPIO_WritePin(MX_PWR_MOTOR_GPIO_Port, MX_PWR_MOTOR_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MX_PWR_ROUTER_GPIO_Port, MX_PWR_ROUTER_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MX_PWR_RADAR_GPIO_Port, MX_PWR_RADAR_Pin, GPIO_PIN_SET);

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_pmu_all_off
 * Description:    关闭所有设备电源
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
uint8_t bsp_pmu_all_off(void) {
    HAL_GPIO_WritePin(MX_PWR_SOC_GPIO_Port, MX_PWR_SOC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MX_PWR_USOUND_GPIO_Port, MX_PWR_USOUND_Pin, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(MX_PWR_MISC1_GPIO_Port, MX_PWR_MISC1_Pin,
    // GPIO_PIN_SET);
    // ///< 任务模块485、IMU、GPS、板内温湿度传感器、电池&驱动器CAN收发芯片
    HAL_GPIO_WritePin(MX_PWR_MISC2_GPIO_Port, MX_PWR_MISC2_Pin,
                      GPIO_PIN_RESET);  ///< 安全触边、水浸和红外接收
    HAL_GPIO_WritePin(MX_PWR_MOTOR_GPIO_Port, MX_PWR_MOTOR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MX_PWR_ROUTER_GPIO_Port, MX_PWR_ROUTER_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MX_PWR_RADAR_GPIO_Port, MX_PWR_RADAR_Pin, GPIO_PIN_RESET);

    return 0;
}

uint8_t bsp_pmu_misc1_off(void) {
    HAL_GPIO_WritePin(MX_PWR_MISC1_GPIO_Port, MX_PWR_MISC1_Pin, GPIO_PIN_SET);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_pmu_misc1_off, bsp_pmu_misc1_off, bsp_pmu_misc1_off);

uint8_t bsp_pmu_misc1_on(void) {
    HAL_GPIO_WritePin(MX_PWR_MISC1_GPIO_Port, MX_PWR_MISC1_Pin, GPIO_PIN_RESET);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_pmu_misc1_on, bsp_pmu_misc1_on, bsp_pmu_misc1_on);

uint8_t bsp_pmu_motor_on(void) {
    HAL_GPIO_WritePin(MX_PWR_MOTOR_GPIO_Port, MX_PWR_MOTOR_Pin, GPIO_PIN_SET);

    return 0;
}

uint8_t bsp_pmu_motor_off(void) {
    HAL_GPIO_WritePin(MX_PWR_MOTOR_GPIO_Port, MX_PWR_MOTOR_Pin, GPIO_PIN_RESET);

    return 0;
}

uint8_t bsp_pmu_heat_on(void) {
    HAL_GPIO_WritePin(MX_PWR_HEAT_GPIO_Port, MX_PWR_HEAT_Pin, GPIO_PIN_SET);

    return 0;
}

uint8_t bsp_pmu_heat_off(void) {
    HAL_GPIO_WritePin(MX_PWR_HEAT_GPIO_Port, MX_PWR_HEAT_Pin, GPIO_PIN_RESET);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} BSP_PMU */
/* @} Robot_BSP */
