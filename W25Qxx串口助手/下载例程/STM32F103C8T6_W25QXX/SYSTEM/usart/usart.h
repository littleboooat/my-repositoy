#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


#define EN_USART1_RX 			1			//ʹ�ܣ�1��/��ֹ��0������1����
#define RECEIVE_DATA_LEN		4096		//���ڽ��ջ�������С
extern u8  USART_RX_BUF[RECEIVE_DATA_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 bound);
#endif


