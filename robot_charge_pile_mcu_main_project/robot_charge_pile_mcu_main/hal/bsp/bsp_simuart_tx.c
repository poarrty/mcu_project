/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         jianyongxiang
 ** Version:        V0.0.1
 ** Date:           2021-7-30
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-07 jianyongxiang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-7-30       jianyongxiang    0.0.1    创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "bsp_simuart_tx.h"
#include <string.h>

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_IR_NEC - 红外通信（模拟串口，38K红外调制）
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
 * Function:       bsp_simuart_tx_init
 * Description:    初始化 bsp_simuart_tx
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

int bsp_simuart_tx_init(SimUart_Tx_TypeDef *simuartx) {
    /*添加模块处理函数*/
    HAL_TIM_PWM_Start(&simuartx->tx_tim, simuartx->tx_channel);

    /*TX引脚初始化为高电平*/
    // HAL_GPIO_WritePin(simuartx->gpio_port, simuartx->gpio_pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 0);
    /*根据波特率设置定时器重装载值*/
    __HAL_TIM_SetAutoreload(&simuartx->tim,
                            simuartx->frq_div_clock / simuartx->baudrate - 1);
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_simuart_tx_deInit
 * Description:    释放 bsp_simuart_tx 资源
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
int bsp_simuart_tx_deInit(void) {
    return 0;
}

static void bsp_simuart_tx_send_data(SimUart_Tx_TypeDef *simuartx,
                                     uint8_t data) {
    HAL_TIM_PWM_Start(&simuartx->tx_tim, simuartx->tx_channel);

    simuartx->tx_status = 1;
    simuartx->SBUF = data;
    HAL_TIM_Base_Stop_IT(&simuartx->tim);
    __HAL_TIM_SetCounter(&simuartx->tim, 0);
    HAL_TIM_Base_Start_IT(&simuartx->tim);
}

uint32_t bsp_simuart_tx_send_buff(SimUart_Tx_TypeDef *simuartx, uint8_t *buff,
                                  uint32_t len) {
    /*发送一包数据过程中，调用发送，发送失败*/
    if (simuartx->tx_status) {
        return 1;
    } else {
        __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 0);
        memcpy(simuartx->send_buff, buff, len);
        simuartx->tx_ptr = 0;
        simuartx->tx_len = len;
        simuartx->buff = simuartx->send_buff;
        bsp_simuart_tx_send_data(simuartx, simuartx->buff[simuartx->tx_ptr++]);
        return 0;
    }
}

static void bsp_simuart_transmit_service(SimUart_Tx_TypeDef *simuartx) {
    /*发送完成，关闭定时器*/
    if (simuartx->tx_ptr == simuartx->tx_len) {
        HAL_TIM_Base_Stop_IT(&simuartx->tim);
        //        HAL_TIM_PWM_Stop(&simuartx->tx_tim, simuartx->tx_channel);
    } else {
        bsp_simuart_tx_send_data(simuartx, simuartx->buff[simuartx->tx_ptr++]);
    }
}

void bsp_simuart_tx_tim_handler(SimUart_Tx_TypeDef *simuartx) {
    if (simuartx->tx_status == 1) {
        simuartx->tx_status++;
    }

    if (simuartx->tx_status == 2) {
        // HAL_GPIO_WritePin(simuartx->gpio_port, simuartx->gpio_pin,
        // GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 631);
        simuartx->tx_status++;
    } else if (simuartx->tx_status < 11) {
        uint8_t currentbit = simuartx->SBUF >> (simuartx->tx_status - 3);

        if (currentbit & 0x01) {
            // HAL_GPIO_WritePin(simuartx->gpio_port, simuartx->gpio_pin,
            // GPIO_PIN_SET);
            __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 0);
        } else {
            // HAL_GPIO_WritePin(simuartx->gpio_port, simuartx->gpio_pin,
            // GPIO_PIN_RESET);
            __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 631);
        }

        simuartx->tx_status++;
    } else if (simuartx->tx_status == 11) {
        // HAL_GPIO_WritePin(simuartx->gpio_port, simuartx->gpio_pin,
        // GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&simuartx->tx_tim, simuartx->tx_channel, 0);
        simuartx->tx_status = 0;
        bsp_simuart_transmit_service(simuartx);
    }

    else {
        simuartx->tx_status = 0;
        bsp_simuart_transmit_service(simuartx);
    }
}

#ifdef __cplusplus
}
#endif

/* @} BSP_SIMUART_TX */
/* @} Robot_BSP */
