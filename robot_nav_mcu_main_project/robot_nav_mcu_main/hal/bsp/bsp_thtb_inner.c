/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-8
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-05 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_thtb_inner.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "i2c.h"

#define LOG_TAG "fal_thtb"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_THTB_INNER - THTB_INNER
 *
 * @brief  内部温湿度\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define THTB_INNER_IIC_H hi2c2

#define CHT_8305_ID   0X8305
#define CHT_8305_ADDR 0X80
#define HDC_1080_ID   0X1050
#define HDC_1080_ADDR 0X40

#define THTB_INNER_ID       CHT_8305_ID
#define THTB_INNER_ADDR     CHT_8305_ADDR
#define THTB_INNER_REG_CFG  0X02
#define THTB_INNER_REG_TEMP 0X00
#define THTB_INNER_REG_ID   0XFF

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
uint8_t  bsp_cht_8305_init(void);
uint8_t  bsp_cht_8305_reset(void);
uint16_t bsp_cht_8305_get_id(void);
uint8_t  bsp_cht_8305_get_data(float *temperature, float *humidity);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_thtb_inner_init
 * Description:    初始化 bsp_thtb_inner
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
int bsp_thtb_inner_init(void) {
    /*添加模块处理函数*/
    bsp_cht_8305_init();

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_thtb_inner_deInit
 * Description:    释放 bsp_thtb_inner 资源
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
int bsp_thtb_inner_deInit(void) {
    return 0;
}

uint8_t bsp_thtb_inner_get_data(float *temperature, float *humidity) {
    bsp_cht_8305_get_data(temperature, humidity);

    return 0;
}

uint8_t bsp_cht_8305_init(void) {
    uint16_t id = 0;

    bsp_cht_8305_reset();

    HAL_Delay(10);

    id = bsp_cht_8305_get_id();

    if (id == THTB_INNER_ID) {
        log_d("THTB inner dev get success, ID[0X%04X]", id);
    } else {
        log_e("THTB inner dev get error, ID[0X%04X]", id);
    }

    ///< 设置转换模式：温度和湿度都进行转换
    uint8_t tx_data[3] = {0};
    tx_data[0]         = THTB_INNER_REG_CFG;
    tx_data[1]         = 0X10;
    tx_data[2]         = 0X00;
    HAL_I2C_Master_Transmit(&THTB_INNER_IIC_H, THTB_INNER_ADDR, tx_data, sizeof(tx_data), 100);

    return 0;
}

uint8_t bsp_cht_8305_reset(void) {
    uint8_t tx_data[3] = {0};

    tx_data[0] = THTB_INNER_REG_CFG;
    tx_data[1] = 0X80;
    tx_data[2] = 0X00;

    HAL_I2C_Master_Transmit(&THTB_INNER_IIC_H, THTB_INNER_ADDR, tx_data, sizeof(tx_data), 100);

    return 0;
}

uint16_t bsp_cht_8305_get_id(void) {
    uint16_t id         = 0;
    uint8_t  tx_data[1] = {0};
    uint8_t  rx_data[2] = {0};

    tx_data[0] = THTB_INNER_REG_ID;

    HAL_I2C_Master_Transmit(&THTB_INNER_IIC_H, THTB_INNER_ADDR, tx_data, sizeof(tx_data), 100);

    HAL_I2C_Master_Receive(&THTB_INNER_IIC_H, THTB_INNER_ADDR, rx_data, sizeof(rx_data), 100);

    id = ((uint16_t) rx_data[0] << 8) + rx_data[1];

    return id;
}

uint8_t bsp_cht_8305_get_data(float *temperature, float *humidity) {
    uint8_t tx_data[1] = {0};
    uint8_t rx_data[4] = {0};

    ///< 写一次温度寄存器地址，触发开始转换（已在初始化配置为温度和湿度都转换，触发后会按顺序进行转换）
    tx_data[0] = THTB_INNER_REG_TEMP;
    HAL_I2C_Master_Transmit(&THTB_INNER_IIC_H, THTB_INNER_ADDR, tx_data, sizeof(tx_data), 100);

    HAL_Delay(10);

    if (HAL_I2C_Master_Receive(&THTB_INNER_IIC_H, THTB_INNER_ADDR, rx_data, sizeof(rx_data), 100) != HAL_OK) {
        return 1;
    }

    uint16_t temp, humi;

    temp = ((uint16_t) rx_data[0] << 8) + rx_data[1];
    humi = ((uint16_t) rx_data[2] << 8) + rx_data[3];

    *temperature = 1.0 * temp * 165 / 65536 - 40;
    *humidity    = 1.0 * humi * 100 / 65536;

    log_d("THTB inner get: %.2f C, %.2f%%", *temperature, *humidity);

    return 0;
}

uint8_t thtb_inner(void) {
    float i, j;

    // bsp_cht_8305_init();

    bsp_cht_8305_get_data(&i, &j);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), thtb_inner, thtb_inner, bsp_cht_8305_get_data);

#ifdef __cplusplus
}
#endif

/* @} BSP_THTB_INNER */
/* @} Robot_BSP */
