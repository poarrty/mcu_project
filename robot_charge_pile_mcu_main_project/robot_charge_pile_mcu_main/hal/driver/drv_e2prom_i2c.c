/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_e2prom_i2c.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 10:54:51
 * @Description: E2PROM底层模拟I2C驱动文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_e2prom_i2c.h"
#include "drv_gpio.h"
#include "delay.h"
#include "gpio.h"

/******************************************************************************
 * @Function: drv_e2prom_i2c_sda_out
 * @Description: 设置I2C_SDA为输出
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_sda_out(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = E2PROM_I2C_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(E2PROM_I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_sda_in
 * @Description: 设置I2C_SDA为输入
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_sda_in(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = E2PROM_I2C_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(E2PROM_I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_start
 * @Description: I2C起始信号：SCL为高时，SDA由高电平变为低电平
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_start(void) {
    drv_e2prom_i2c_sda_out();
    E2PROM_I2C_SDA_H();
    E2PROM_I2C_SCL_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SDA_L();
    delay_us(E2PROM_I2C_DELAY_VAL);
    //钳住I2C总线，准备发送或接收数据
    E2PROM_I2C_SCL_L();
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_stop
 * @Description: I2C停止信号：SCL为高时，SDA由低电平变为高电平
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_stop(void) {
    drv_e2prom_i2c_sda_out();
    E2PROM_I2C_SDA_L();
    E2PROM_I2C_SCL_L();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_H();
    E2PROM_I2C_SDA_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_wait_ack
 * @Description: 等待ACK信号：SCL为高时，SDA为低电平
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
uint8_t drv_e2prom_i2c_wait_ack(void) {
    uint8_t timeout = 0;

    drv_e2prom_i2c_sda_in();
    ;
    E2PROM_I2C_SDA_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_H();
    delay_us(E2PROM_I2C_DELAY_VAL);

    while (E2PROM_I2C_SDA_READ()) {
        timeout++;

        if (timeout > E2PROM_I2C_TIMEOUT_DELAY_VAL) {
            drv_e2prom_i2c_stop();
            return 1;
        }
    }

    E2PROM_I2C_SCL_L();

    return 0;
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_send_ack
 * @Description: 发送ACK信号：SCL为高时，SDA为低电平
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_send_ack(void) {
    E2PROM_I2C_SCL_L();
    drv_e2prom_i2c_sda_out();
    E2PROM_I2C_SDA_L();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_L();
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_send_nack
 * @Description: 发送NACK信号：SCL为高时，SDA为高电平
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_e2prom_i2c_send_nack(void) {
    E2PROM_I2C_SCL_L();
    drv_e2prom_i2c_sda_out();
    E2PROM_I2C_SDA_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_H();
    delay_us(E2PROM_I2C_DELAY_VAL);
    E2PROM_I2C_SCL_L();
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_write_byte
 * @Description: 发送一个字节的数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} data
 *******************************************************************************/
void drv_e2prom_i2c_write_byte(uint8_t data) {
    uint8_t i = 0;

    drv_e2prom_i2c_sda_out();
    E2PROM_I2C_SCL_L();

    for (i = 0; i < 8; i++) {
        if ((data & 0x80) >> 7) {
            E2PROM_I2C_SDA_H();
        } else {
            E2PROM_I2C_SDA_L();
        }

        data <<= 1;
        delay_us(E2PROM_I2C_DELAY_VAL);
        E2PROM_I2C_SCL_H();
        delay_us(E2PROM_I2C_DELAY_VAL);
        E2PROM_I2C_SCL_L();
        delay_us(E2PROM_I2C_DELAY_VAL);
    }
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_read_byte
 * @Description: 接收一个字节的数据
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} ack
 *******************************************************************************/
uint8_t drv_e2prom_i2c_read_byte(uint8_t ack) {
    uint8_t i = 0;
    uint8_t data = 0;

    drv_e2prom_i2c_sda_in();

    for (i = 0; i < 8; i++) {
        E2PROM_I2C_SCL_L();
        delay_us(E2PROM_I2C_DELAY_VAL);
        E2PROM_I2C_SCL_H();
        data <<= 1;

        if (E2PROM_I2C_SDA_READ()) {
            data++;
        }

        delay_us(E2PROM_I2C_DELAY_VAL);
    }

    if (!ack) {
        drv_e2prom_i2c_send_nack();
    } else {
        drv_e2prom_i2c_send_ack();
    }

    return data;
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_write_one_byte
 * @Description: 写E2PROM一个字节
 * @Input: device_addr：设备地址
 *         reg_addr：寄存器地址
 *         data：要写入的数据
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} device_addr
 * @param {uint8_t} reg_addr
 * @param {uint16_t} data
 *******************************************************************************/
void drv_e2prom_i2c_write_one_byte(uint8_t device_addr, uint16_t reg_addr,
                                   uint8_t data) {
    //获取写入地址的高3位并左移一位
    uint8_t page_num = (((reg_addr >> 8) & 0xFF) << 1);

    drv_e2prom_i2c_start();
    drv_e2prom_i2c_write_byte(device_addr | page_num);
    drv_e2prom_i2c_wait_ack();
    drv_e2prom_i2c_write_byte(reg_addr & 0xFF);
    drv_e2prom_i2c_wait_ack();
    drv_e2prom_i2c_write_byte(data);
    drv_e2prom_i2c_wait_ack();
    drv_e2prom_i2c_stop();
    //此处必须要有延时：1.9ms~3ms
    HAL_Delay(E2PROM_I2C_CYCLE_WRITE_DELAY_VAL);
}

/******************************************************************************
 * @Function: drv_e2prom_i2c_read_one_byte
 * @Description: 读E2PROM一个字节
 * @Input: device_addr：设备地址
 *         reg_addr：要读取的寄存器地址
 *         data：存放读取的数据的地址
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} device_addr
 * @param {uint8_t} reg_addr
 * @param {uint16_t} *data
 *******************************************************************************/
void drv_e2prom_i2c_read_one_byte(uint8_t device_addr, uint16_t reg_addr,
                                  uint8_t *data) {
    //获取写入地址的高3位并左移一位
    uint8_t page_num = (((reg_addr >> 8) & 0xFF) << 1);

    drv_e2prom_i2c_start();
    drv_e2prom_i2c_write_byte(device_addr | page_num);
    drv_e2prom_i2c_wait_ack();
    drv_e2prom_i2c_write_byte(reg_addr & 0xFF);
    drv_e2prom_i2c_wait_ack();
    drv_e2prom_i2c_start();
    drv_e2prom_i2c_write_byte(device_addr | 0x01);
    drv_e2prom_i2c_wait_ack();
    *data = drv_e2prom_i2c_read_byte(0);
    drv_e2prom_i2c_stop();
}
