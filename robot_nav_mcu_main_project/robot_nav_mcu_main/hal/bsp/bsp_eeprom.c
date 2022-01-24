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
#include "bsp_eeprom.h"
#include "drv_soft_i2c.h"
#include "delay.h"
#include "main.h"
#include "i2c.h"
#include "wwdg.h"
#include "elog.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_EEPROM - BSP_EEPROM
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
#define delay_ms HAL_Delay

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767
// STM32F429开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02

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
 * Function:       bsp_eeprom_init
 * Description:    初始化 bsp_eeprom
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
int bsp_eeprom_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_eeprom_deInit
 * Description:    释放 bsp_eeprom 资源
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
int bsp_eeprom_deInit(void) {
    return 0;
}

//初始化IIC接口
void AT24CXX_Init(void) {
    // IIC_Init();//IIC初始化
}
//在AT24CXX指定地址读出一个数据
// ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr) {
    u8 temp = 0;
    IIC_Start();

    if (EE_TYPE > AT24C16) {
        IIC_Send_Byte(0XA0);  //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr >> 8);  //发送高地址
    } else {
        IIC_Send_Byte(0XA0 + ((ReadAddr / 256) << 1));
    }  //发送器件地址0XA0,写数据

    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr % 256);  //发送低地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1);  //进入接收模式
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();  //产生一个停止条件
    return temp;
}
//在AT24CXX指定地址写入一个数据
// WriteAddr  :写入数据的目的地址
// DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite) {
    IIC_Start();

    if (EE_TYPE > AT24C16) {
        IIC_Send_Byte(0XA0);  //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr >> 8);  //发送高地址
    } else {
        IIC_Send_Byte(0XA0 + ((WriteAddr / 256) << 1));
    }  //发送器件地址0XA0,写数据

    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr % 256);  //发送低地址
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);  //发送字节
    IIC_Wait_Ack();
    IIC_Stop();  //产生一个停止条件
    delay_ms(10);
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
// WriteAddr  :开始写入的地址
// DataToWrite:数据数组首地址
// Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len) {
    u8 t;

    for (t = 0; t < Len; t++) {
        AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
    }
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
// ReadAddr   :开始读出的地址
//返回值     :数据
// Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len) {
    u8  t;
    u32 temp = 0;

    for (t = 0; t < Len; t++) {
        temp <<= 8;
        temp += AT24CXX_ReadOneByte(ReadAddr + Len - t - 1);
    }

    return temp;
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void) {
    u8 temp;
    temp = AT24CXX_ReadOneByte(255);  //避免每次开机都写AT24CXX

    if (temp == 0X55) {
        return 0;
    } else  //排除第一次初始化的情况
    {
        AT24CXX_WriteOneByte(255, 0X55);
        temp = AT24CXX_ReadOneByte(255);

        if (temp == 0X55) {
            return 0;
        }
    }

    return 1;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
// ReadAddr :开始读出的地址 对24c02为0~255
// pBuffer  :数据数组首地址
// NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead) {
    while (NumToRead) {
        *pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}
//在AT24CXX里面的指定地址开始写入指定个数的数据
// WriteAddr :开始写入的地址 对24c02为0~255
// pBuffer   :数据数组首地址
// NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite) {
    while (NumToWrite--) {
        AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

//在BL24C16指定地址读出一个数据
// ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 BL24C16_ReadOneByte(u16 ReadAddr) {
    uint8_t data;
    uint8_t devAddr = 0XA0 + ((ReadAddr / 256) << 1);
    uint8_t regAddr = ReadAddr % 256;

    HAL_I2C_Mem_Read(&hi2c1, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    return data;
}
//在BL24C16指定地址写入一个数据
// WriteAddr  :写入数据的目的地址
// DataToWrite:要写入的数据
void BL24C16_WriteOneByte(u16 WriteAddr, u8 DataToWrite) {
    uint8_t data    = DataToWrite;
    uint8_t devAddr = 0XA0 + ((WriteAddr / 256) << 1);
    uint8_t regAddr = WriteAddr % 256;

    HAL_I2C_Mem_Write(&hi2c1, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    // HAL_Delay(5);     ///< 用这个延时会导致在中断里写不了数据
    delay_us(5000);
}

void BL24C16_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead) {
    while (NumToRead) {
        HAL_WWDG_Refresh(&hwwdg);
        *pBuffer++ = BL24C16_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

void BL24C16_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite) {
    while (NumToWrite--) {
        HAL_WWDG_Refresh(&hwwdg);
        BL24C16_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

#ifdef __cplusplus
}
#endif

/* @} BSP_EEPROM */
/* @} Robot_BSP */
