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
#include "bsp_simuart_rx.h"
#include "gpio.h"
#include "string.h"
#include "shell.h"
#include "tim.h"
#include "gpio.h"

#define LOG_TAG "bsp_simuart_rx"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_SIMUART_RX - 红外通信（模拟串口，38K红外调制）
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
#define ENABLE  1
#define DISABLE 0
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
 * Function:       bsp_simuart_rx_init
 * Description:    初始化 bsp_simuart_rx
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

int bsp_simuart_rx_init(SimUart_Rx_TypeDef *simuartx) {
    /*添加模块处理函数*/
    /*模拟串口RX脚应设置为上拉输入，下降沿触发外部触发外部中断*/

    /**/
    simuartx->state = ENABLE;
    /*根据波特率设置定时器重装载值*/
    __HAL_TIM_SetAutoreload(&simuartx->tim, simuartx->frq_div_clock / simuartx->baudrate - 1);

    /*修改重装载值后，第一次定时时间不准？先启动一次定时器*/
    HAL_TIM_Base_Start_IT(&simuartx->tim);
    HAL_TIM_Base_Stop_IT(&simuartx->tim);
    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_simuart_rx_deInit
 * Description:    释放 bsp_simuart_rx 资源
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
int bsp_simuart_rx_deInit(void) {
    return 0;
}

/*使能模拟串口接受*/
void bsp_simuart_rx_enable(SimUart_Rx_TypeDef *simuartx) {
    simuartx->state = ENABLE;
}

/*关闭模拟串口接受*/
void bsp_simuart_rx_disable(SimUart_Rx_TypeDef *simuartx) {
    simuartx->state = DISABLE;
}

/*定时器处理函数*/
void bsp_simuart_rx_tim_handler(SimUart_Rx_TypeDef *simuartx) {
    uint8_t currentbit = HAL_GPIO_ReadPin(simuartx->gpio_port, simuartx->gpio_pin);

    if (simuartx->rx_status == 1) {
        simuartx->rx_status++;
    } else if ((simuartx->rx_status < 10) && (simuartx->rx_status > 1)) {
        simuartx->SBUF >>= 1;

        if (currentbit != GPIO_PIN_RESET) {
            simuartx->SBUF |= 0x80;
        }

        simuartx->rx_status++;
    } else if (simuartx->rx_status == 10) {
        simuartx->rx_status = 0;
        HAL_TIM_Base_Stop_IT(&simuartx->tim);

        /*不考虑奇偶校验，此处为判断停止位*/
        if (currentbit == GPIO_PIN_SET) {
            if (simuartx->rx_ptr < BUFF_LEN) {
                simuartx->rxbuff[simuartx->rx_ptr++] = simuartx->SBUF;
            } else {
                if (simuartx->rx_getpack) {
                    log_e("simuart rxbuff too much, rx pin is %x", simuartx->gpio_pin);
                }
            }

            simuartx->pack_timeout = simuartx->get_timecount();
            simuartx->receive_callback(simuartx->SBUF);
        }
    } else {
    }
}

/*当get_timecount能获取时钟时，可以用此函数获取一帧数据，需要轮询调用*/
void bsp_simuart_rx_getdata(SimUart_Rx_TypeDef *simuartx, uint8_t *buff, uint8_t *len, uint32_t timeout) {
    if (simuartx->rx_getpack == 0) {
        simuartx->rx_getpack = 1;
    }

    if ((simuartx->get_timecount() - simuartx->pack_timeout) >= timeout) {
        memcpy(buff, simuartx->rxbuff, simuartx->rx_ptr);
        *len                 = simuartx->rx_ptr;
        simuartx->rx_ptr     = 0;
        simuartx->rx_getpack = 0;
    }
}

/*外部中断处理函数*/
void bsp_simuart_rx_exti_handle(SimUart_Rx_TypeDef *simuartx) {
    if (HAL_GPIO_ReadPin(simuartx->gpio_port, simuartx->gpio_pin) == GPIO_PIN_RESET) {
        if ((simuartx->rx_status == 0) && (simuartx->state == ENABLE)) {
            simuartx->rx_status++;
            simuartx->SBUF = 0;
            __HAL_TIM_SetCounter(&simuartx->tim, (simuartx->frq_div_clock / simuartx->baudrate - 1) >> 1);
            HAL_TIM_Base_Start_IT(&simuartx->tim);
        }
    }
}
#ifdef __cplusplus
}
#endif

/* @} BSP_SIMUART_RX */
/* @} Robot_BSP */
