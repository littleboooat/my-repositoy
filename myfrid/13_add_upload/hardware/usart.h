#ifndef __USART_DRIVER_H
#define __USART_DRIVER_H
#include "stm32f10x.h" 
void usart_int(void);
void usart2_int(void);
void Serial_Printf(const char *format, ...);

void Serial_SendString(USART_TypeDef* USARTx,char *String);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
#endif
//#ifndef _usarta_h
//#define _usarta_h
//void Serial_Printf(char *format, ...);
//void ini_usart(void);
//void Usart2_Init(void);
//void sendstring(char *p);
////#include <stdio.h>
//void Serial_Printf(char *format, ...);
//void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
//void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
//#endif
