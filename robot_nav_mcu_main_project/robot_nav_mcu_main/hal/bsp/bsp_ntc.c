/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-6-22
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-06 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_ntc.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "adc.h"
#include "math.h"
#include "stdio.h"

#define LOG_TAG "bsp_ntc"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_NTC - BSP_NTC
 *
 * @brief  热敏电阻测温\n
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
static uint16_t bsp_ntc_adc(uint16_t *val);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_ntc_init
 * Description:    初始化 bsp_ntc_init
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
int bsp_ntc_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_ntc_deInit
 * Description:    释放 bsp_ntc_deInit 资源
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
int bsp_ntc_deInit(void) {
    return 0;
}

uint8_t bsp_ntc_get_data(float *temperature) {
    //常数B
    float B = 4150;
    //常温(25℃)下热敏电阻的标称阻值阻值
    float Rp = 47000;
    //分压电阻的阻值
    float Rd = 2000;
    //开尔文温度
    float K = 237.15;
    // 25℃对应的华摄氏度
    float T2 = K + 25;

    //热敏电阻 ADC 采样值
    uint16_t ntc_adc_val = 0;
    //热敏电阻 采样电压值
    float ntc_vol_val = 0.0;
    //热敏电阻 当前阻值
    float ntc_reg_val = 0.0;
    //热敏电阻 当前温度
    float ntc_temp_val = 0.0;

    if (bsp_ntc_adc(&ntc_adc_val) == 0) {
        ntc_vol_val  = (float) ntc_adc_val / 4096 * 3.3f;
        ntc_reg_val  = (Rd * ntc_vol_val) / (3.3f - ntc_vol_val);
        ntc_temp_val = (1 / (1 / T2 + log(ntc_reg_val / Rp) / B)) - K + 0.5;

        *temperature = ntc_temp_val;

        log_i("NTC: adc[%d] vol[%.2f] reg[%.2f] temp[%.2f]", ntc_adc_val, ntc_vol_val, ntc_reg_val, ntc_temp_val);

        return 0;
    }

    else {
        log_e("NTC adc error");

        return 1;
    }
}

uint8_t bsp_ntc_temp(void) {
    float temp;
    bsp_ntc_get_data(&temp);
    printf("temp : %.1f'C\r\n", temp);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_ntc_temp, bsp_ntc_temp, bsp_ntc_temp);

static uint16_t bsp_ntc_adc(uint16_t *val) {
    ADC_ChannelConfTypeDef adc1_channel_config;
    HAL_StatusTypeDef      status;

    adc1_channel_config.Channel      = ADC_CHANNEL_9;
    adc1_channel_config.Rank         = 1;
    adc1_channel_config.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    status                           = HAL_ADC_ConfigChannel(&hadc1, &adc1_channel_config);

    if (status != HAL_OK) {
        // LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_ADC_Start(&hadc1);

    if (status != HAL_OK) {
        // LOG_ERROR("Return val:%d", status);
        return status;
    }

    status = HAL_ADC_PollForConversion(&hadc1, 100);

    if (status != HAL_OK) {
        // LOG_ERROR("Return val:%d", status);
        return status;
    }

    *val = (uint16_t) HAL_ADC_GetValue(&hadc1);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} BSP_THTB_INNER */
/* @} Robot_BSP */
