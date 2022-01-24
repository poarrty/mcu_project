/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         jianyongxiang
 ** Version:        V0.0.1
 ** Date:           2021-7-2
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-07 jianyongxiang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-7-2      jianyongxiang   0.0.1      创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_tough_edge.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "adc.h"
#include "math.h"
#include "cmsis_os.h"
#define LOG_TAG "bsp_touch_edge"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_TOUGH_EDGE
 *
 * @brief  触边检测\n
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
static uint16_t bsp_tough_edge_error_flag;
/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static uint16_t bsp_tough_edge_adc(uint16_t *val);

/*****************************************************************
 * 函数定义
 ******************************************************************/
uint16_t bsp_get_tough_edge_error_flag(void);
/*****************************************************************/
/**
 * Function:       bsp_tough_edge_init
 * Description:    初始化 bsp_tough_edge_init
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
int bsp_tough_edge_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_tough_edge_deInit
 * Description:    释放 bsp_tough_edge_deInit 资源
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
int bsp_tough_edge_deInit(void) {
    return 0;
}

uint8_t bsp_gp2y0_get_data(uint16_t *tough_edge_front_adc_val, float *tough_edge_front_distance_val, uint16_t *tough_edge_back_adc_val,
                           float *tough_edge_back_distance_val) {
    //触边传感器 ADC 采样值
    uint16_t tough_edge_adc_val[2]     = {0};
    float    tough_edge_vlotage_val[2] = {0};

    if (bsp_tough_edge_adc(tough_edge_adc_val) == 0) {
        for (uint8_t i = 0; i < 2; i++) {
            if ((tough_edge_adc_val[i] < 15) || (tough_edge_adc_val[i] >= 4000)) {
                bsp_tough_edge_error_flag |= (1 << i);
            } else {
                bsp_tough_edge_error_flag &= (uint16_t) ~(1 << i);
            }
        }

        *tough_edge_front_adc_val = tough_edge_adc_val[0];
        tough_edge_vlotage_val[0] = tough_edge_adc_val[0] * 3.3 / 4096;

        /*根据数据用excel做分段拟合*/
        if (tough_edge_vlotage_val[0] > 2.23f) {
            *tough_edge_front_distance_val = 1;
        } else if (tough_edge_vlotage_val[0] > 0.52f) {
            *tough_edge_front_distance_val = -3.6096f * pow(tough_edge_vlotage_val[0], 3) + 18.659f * pow(tough_edge_vlotage_val[0], 2) -
                                             33.334f * tough_edge_vlotage_val[0] + 22.54;
        } else if (tough_edge_vlotage_val[0] > 0.24f) {
            *tough_edge_front_distance_val = 63.246f * pow(tough_edge_vlotage_val[0], 2) - 82.188f * tough_edge_vlotage_val[0] + 35.74f;
        } else {
            *tough_edge_front_distance_val = 933.66f * pow(tough_edge_vlotage_val[0], 2) - 437.61f * tough_edge_vlotage_val[0] + 71.309f;
        }

        *tough_edge_back_adc_val  = tough_edge_adc_val[1];
        tough_edge_vlotage_val[1] = tough_edge_adc_val[1] * 3.3 / 4096;

        /*根据数据用excel做分段拟合*/
        if (tough_edge_vlotage_val[1] > 2.23f) {
            *tough_edge_back_distance_val = 1;
        } else if (tough_edge_vlotage_val[1] > 0.52f) {
            *tough_edge_back_distance_val = -3.6096f * pow(tough_edge_vlotage_val[1], 3) + 18.659f * pow(tough_edge_vlotage_val[1], 2) -
                                            33.334f * tough_edge_vlotage_val[1] + 22.54;
        } else if (tough_edge_vlotage_val[1] > 0.24f) {
            *tough_edge_back_distance_val = 63.246f * pow(tough_edge_vlotage_val[1], 2) - 82.188f * tough_edge_vlotage_val[1] + 35.74f;
        } else {
            *tough_edge_back_distance_val = 933.66f * pow(tough_edge_vlotage_val[1], 2) - 437.61f * tough_edge_vlotage_val[1] + 71.309f;
        }

        log_d("tough_edge_front: adc[%d] vol[%.2f] dis[%.2f]\n", *tough_edge_front_adc_val, tough_edge_vlotage_val[0],
              *tough_edge_front_distance_val);
        log_d("tough_edge_back: adc[%d] vol[%.2f] dis[%.2f]\n", *tough_edge_back_adc_val, tough_edge_vlotage_val[1],
              *tough_edge_back_distance_val);

        return 0;
    }

    else {
        log_e("tough_edge adc error");

        return 1;
    }
}

uint8_t bsp_tough_edge_get_data(uint16_t *tough_edge_front_adc_val, float *tough_edge_front_distance_val, uint16_t *tough_edge_back_adc_val,
                                float *tough_edge_back_distance_val) {
    bsp_gp2y0_get_data(tough_edge_front_adc_val, tough_edge_front_distance_val, tough_edge_back_adc_val, tough_edge_back_distance_val);

    return 0;
}

uint8_t bsp_tough_edge_temp(void) {
    uint16_t tough_edge_adc_val_front;
    float    tough_edge_distance_val_front;
    uint16_t tough_edge_adc_val_back;
    float    tough_edge_distance_val_back;

    bsp_gp2y0_get_data(&tough_edge_adc_val_front, &tough_edge_distance_val_front, &tough_edge_adc_val_back, &tough_edge_distance_val_back);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_tough_edge_temp, bsp_tough_edge_temp, bsp_gp2y0_get_data);

uint16_t bsp_get_tough_edge_error_flag(void) {
    return bsp_tough_edge_error_flag;
}

static uint16_t bsp_tough_edge_adc(uint16_t *val) {
    ADC_ChannelConfTypeDef adc1_channel_config;
    HAL_StatusTypeDef      status;
    /*PC4， 暂时设定为左，通道可更改*/
    adc1_channel_config.Channel      = ADC_CHANNEL_14;
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

    val[0] = (uint16_t) HAL_ADC_GetValue(&hadc1);
    /*PC5， 暂时设定为右，通道可更改*/
    adc1_channel_config.Channel      = ADC_CHANNEL_15;
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

    val[1] = (uint16_t) HAL_ADC_GetValue(&hadc1);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} BSP_THTB_INNER */
/* @} Robot_BSP */
