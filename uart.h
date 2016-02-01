/* 
 * File:   uart.h
 * Author: lal
 *
 * Created on 9 avril 2015, 07:42
 */

#ifndef _UART_H
#define	_UART_H

#include "general.h"


void UARTInit(void);           // init UART with specified baud rate
UINT8_T UARTReadByte(void);                 // read a byte from UART
void UARTWriteByte(UINT8_T data);           // write a byte to UART
void UARTBasicCommand();
void UARTConfigBaudRate(UINT8_T baudRateChosen);
void UARTWriteChar(char data);
void UARTWakeUp();

#endif	/* _UART_H */

