#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

void Serial_Init(uint8_t usart_num, uint32_t baud_rate);
void Serial_SendByte(uint8_t usart_num, uint8_t Byte);
void Serial_SendArray(uint8_t usart_num, uint8_t *Array, uint16_t Length);
void Serial_SendString(uint8_t usart_num, char *String);
void Serial_SendNumber(uint8_t usart_num, uint32_t Number, uint8_t Length);
void Serial_Printf(uint8_t usart_num, char *format, ...);

uint8_t Serial_GetRxFlag(uint8_t usart_num);
uint8_t Serial_GetRxData(uint8_t usart_num);

#endif



