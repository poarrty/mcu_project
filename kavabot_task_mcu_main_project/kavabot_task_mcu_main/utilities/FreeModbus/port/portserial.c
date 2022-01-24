/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "usart.h"
#include "main.h"
#include <stdint.h>
//#include "stm32f1xx_ll_usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
void prvvUARTTxReadyISR(void);
void prvvUARTRxISR(void);

// uint8_t modbus_usart_rx_data;
// uint8_t modbus_usart_tx_data;

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */

    // STM32串口接收中断使能
    if (xRxEnable == TRUE) {
        //使能接收中断
        // HAL_UART_Receive_IT(&huart2, &modbus_usart_rx_data, 1);
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    } else {
        //禁止接收中断
        // HAL_UART_AbortReceive_IT(&huart2);
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    }

    // STM32串口发送中断使能
    if (xTxEnable == TRUE) {
        //使能发送中断
        // LL_USART_EnableIT_TXE(huart2.Instance);

        //通知MODBUS库发送缓冲区为空
        // prvvUARTTxReadyISR();

        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
    } else {
        // LL_USART_DisableIT_TXE(huart2.Instance);
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
    }
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                       eMBParity eParity) {
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte) {
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */

    // modbus_usart_tx_data = ucByte;
    // HAL_UART_Transmit_IT(&huart2, &modbus_usart_tx_data, 1);

    // return TRUE;

    if (HAL_UART_Transmit(&huart2, (uint8_t *) &ucByte, 1, 100) != HAL_OK) {
        return FALSE;
    } else {
        return TRUE;
    }
}

BOOL xMBPortSerialGetByte(CHAR *pucByte) {
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */

    // *pucByte = modbus_usart_rx_data;
    // HAL_UART_Receive_IT(&huart2, &modbus_usart_rx_data, 1);

    // return TRUE;

    if (HAL_UART_Receive(&huart2, (uint8_t *) pucByte, 1, 100) != HAL_OK) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void) {
    pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void) {
    pxMBFrameCBByteReceived();
}
