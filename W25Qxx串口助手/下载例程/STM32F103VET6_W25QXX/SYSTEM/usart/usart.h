#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


#define EN_USART1_RX 			1			//使能（1）/禁止（0）串口1接收
#define RECEIVE_DATA_LEN		4096		//串口接收缓存区大小
extern u8  USART_RX_BUF[RECEIVE_DATA_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart_init(u32 bound);
#endif


