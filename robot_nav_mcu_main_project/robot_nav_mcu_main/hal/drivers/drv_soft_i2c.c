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
#include "drv_soft_i2c.h"
#include "main.h"
#include "delay.h"

/**
 * @addtogroup Robot_DRV
 * @{
 */

/**
 * @defgroup DRV_SOFT_I2C 软件I2C - SOFT-I2C
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
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
// IO口操作宏定义
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *) (addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOB_ODR_Addr (GPIOB_BASE + 20)  // 0x40020414
#define GPIOB_IDR_Addr (GPIOB_BASE + 16)  // 0x40020410
// IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)  //输出
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)  //输入
// IO方向设置
#define SDA_IN()                         \
    {                                    \
        GPIOB->MODER &= ~(3 << (7 * 2)); \
        GPIOB->MODER |= 0 << 7 * 2;      \
    }  // PB9输入模式
#define SDA_OUT()                        \
    {                                    \
        GPIOB->MODER &= ~(3 << (7 * 2)); \
        GPIOB->MODER |= 1 << 7 * 2;      \
    }  // PB9输出模式
// IO操作
#define IIC_SCL  PBout(6)  // SCL
#define IIC_SDA  PBout(7)  // SDA
#define READ_SDA PBin(7)   //输入SDA

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
 * Function:       drv_soft_i2c_init
 * Description:    初始化 drv_soft_i2c
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
int drv_soft_i2c_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       drv_soft_i2c_deInit
 * Description:    释放 drv_soft_i2c 资源
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
int drv_soft_i2c_deInit(void) {
    return 0;
}

//产生IIC起始信号
void IIC_Start(void) {
    SDA_OUT();  // sda线输出
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;  // START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL = 0;  //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void) {
    SDA_OUT();  // sda线输出
    IIC_SCL = 0;
    IIC_SDA = 0;  // STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL = 1;
    IIC_SDA = 1;  //发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void) {
    u8 ucErrTime = 0;
    SDA_IN();  // SDA设置为输入
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);

    while (READ_SDA) {
        ucErrTime++;

        if (ucErrTime > 250) {
            IIC_Stop();
            return 1;
        }
    }

    IIC_SCL = 0;  //时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void) {
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}
//不产生ACK应答
void IIC_NAck(void) {
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}
// IIC发送一个字节
//返回从机有无应答
// 1，有应答
// 0，无应答
void IIC_Send_Byte(u8 txd) {
    u8 t;
    SDA_OUT();
    IIC_SCL = 0;  //拉低时钟开始数据传输

    for (t = 0; t < 8; t++) {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);  //对TEA5767这三个延时都是必须的
        IIC_SCL = 1;
        delay_us(2);
        IIC_SCL = 0;
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    SDA_IN();  // SDA设置为输入

    for (i = 0; i < 8; i++) {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;

        if (READ_SDA) {
            receive++;
        }

        delay_us(1);
    }

    if (!ack) {
        IIC_NAck();  //发送nACK
    } else {
        IIC_Ack();  //发送ACK
    }

    return receive;
}

#ifdef __cplusplus
}
#endif

/* @} DRV_SOFT_I2C */
/* @} Robot_FAL */
