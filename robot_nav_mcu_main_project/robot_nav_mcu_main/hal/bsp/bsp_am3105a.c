/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-5-10
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
#include "bsp_am3105a.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "i2c.h"
#include "delay.h"

#define LOG_TAG "fal_thtb"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_AM3105A - AM3105A
 *
 * @brief  温湿度传感器AM3105A\n
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
static void    bsp_am3105a_reset(void);
static uint8_t bsp_am3105a_check(void);
static uint8_t bsp_am3105a_read_bit(void);
static uint8_t bsp_am3105a_read_byte(void);
static void    bsp_am3105a_sda_mode_in(void);
static void    bsp_am3105a_sda_mode_out(void);
static uint8_t bsp_am3105a_sda_in(void);
static void    bsp_am3105a_sda_out(uint8_t val);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_am3105a_init
 * Description:    初始化 bsp_am3105a
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
int bsp_am3105a_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_am3105a_deInit
 * Description:    释放 bsp_am3105a 资源
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
int bsp_am3105a_deInit(void) {
    return 0;
}

uint8_t bsp_am3105a_get_data(float *temperature, float *humidity) {
    uint8_t buff[5] = {0};
    uint8_t i;
    uint8_t ret = 1;
    uint8_t checksum;

    uint16_t temp, humi;

    bsp_am3105a_reset();

    if (bsp_am3105a_check() == 0) {
        __disable_irq();

        for (i = 0; i < 5; i++) {
            buff[i] = bsp_am3105a_read_byte();
        }

        __enable_irq();

        checksum = 0;
        checksum += buff[0];
        checksum += buff[1];
        checksum += buff[2];
        checksum += buff[3];

        if (checksum == buff[4]) {
            humi = ((uint16_t) buff[0] << 8) + buff[1];
            temp = ((uint16_t) buff[2] << 8) + buff[3];

            if (humi & 0X8000) {
                humi      = humi & (~0X8000);
                *humidity = -1.0 * humi / 10;
            } else {
                *humidity = 1.0 * humi / 10;
            }

            if (temp & 0X8000) {
                temp         = temp & (~0X8000);
                *temperature = -1.0 * temp / 10;
            } else {
                *temperature = 1.0 * temp / 10;
            }

            log_d("AM3105A data : temperature[%.2f C] humidity[%.2f%%]", *temperature, *humidity);

        } else {
            // elog_hexdump("AM3105A data", sizeof(buff), buff, sizeof(buff));
            log_e("AM3105A data checksum error, want[0X%02X] GET[0X%02X]", buff[4], checksum);
            ret = 1;
        }

    } else {
        log_e("AM3105A not found.");

        ret = 1;
    }

    return ret;
}

void bsp_am3105a_reset(void) {
    bsp_am3105a_sda_mode_out();

    ///< 主机起始信号拉低时间/ms min: 0.8 typ:1 max: 20
    bsp_am3105a_sda_out(0);
    HAL_Delay(2);

    ///< 主机释放总线时间/us min: 10 typ:30 max:200
    bsp_am3105a_sda_out(1);
    delay_us(30);
}

static uint8_t bsp_am3105a_check(void) {
    uint8_t retry = 0;

    bsp_am3105a_sda_mode_in();

    ///< 从机响应低电平时间/us min: 75 typ:80 max:85
    ///< 等待低电平信号，100 us 超时
    while (bsp_am3105a_sda_in() && retry < 100) {
        retry++;
        delay_us(1);
    }

    if (retry >= 100) {
        return 1;
    }

    retry = 0;

    ///< 从机响应高电平时间/us min: 75 typ:80 max:85
    ///< 等待高电平信号，100 us 超时
    while (!bsp_am3105a_sda_in() && retry < 100) {
        retry++;
        delay_us(1);
    }

    ///< 等待响应电平结束
    // while(bsp_am3105a_sda_in());

    if (retry >= 100) {
        return 1;
    } else {
        return 0;
    }
}

static uint8_t bsp_am3105a_read_bit(void) {
    uint8_t retry = 0;

    ///< 信号 "0"/"1" 低电平时间/us min: 48 typ:50 max:55
    ///< 等待低电平信号，100 us 超时
    while (bsp_am3105a_sda_in() && retry < 100) {
        retry++;
        delay_us(1);
    }

    retry = 0;

    ///< 信号 "0" 高电平时间/us min: 22 typ:26 max:30
    ///< 信号 "1" 高电平时间/us min: 68 typ:70 max:75
    ///< 等待高电平信号，100 us 超时
    while (!bsp_am3105a_sda_in() && retry < 100) {
        retry++;
        delay_us(1);
    }

    ///< 信号采样
    delay_us(40);

    if (bsp_am3105a_sda_in()) {
        return 1;
    } else {
        return 0;
    }
}

static uint8_t bsp_am3105a_read_byte(void) {
    uint8_t i, data;

    data = 0;

    for (i = 0; i < 8; i++) {
        data <<= 1;
        data |= bsp_am3105a_read_bit();
    }

    return data;
}

static void bsp_am3105a_sda_mode_in(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin  = MX_1WIRE_THTB_EXT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MX_1WIRE_THTB_EXT_GPIO_Port, &GPIO_InitStruct);
}

static void bsp_am3105a_sda_mode_out(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin   = MX_1WIRE_THTB_EXT_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MX_1WIRE_THTB_EXT_GPIO_Port, &GPIO_InitStruct);
}

static uint8_t bsp_am3105a_sda_in(void) {
    if (HAL_GPIO_ReadPin(MX_1WIRE_THTB_EXT_GPIO_Port, MX_1WIRE_THTB_EXT_Pin) == GPIO_PIN_SET) {
        return 1;
    }

    else {
        return 0;
    }
}

static void bsp_am3105a_sda_out(uint8_t val) {
    if (val) {
        HAL_GPIO_WritePin(MX_1WIRE_THTB_EXT_GPIO_Port, MX_1WIRE_THTB_EXT_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(MX_1WIRE_THTB_EXT_GPIO_Port, MX_1WIRE_THTB_EXT_Pin, GPIO_PIN_RESET);
    }
}

#ifdef __cplusplus
}
#endif

/* @} BSP_AM3105A */
/* @} Robot_BSP */
