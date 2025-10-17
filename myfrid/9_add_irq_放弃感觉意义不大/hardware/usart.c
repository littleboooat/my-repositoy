#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include"semphr.h"
#include <stdarg.h>   // 提供 va_list, va_start, va_end
#include <stdio.h>    // 提供 vsnprintf()




void usart_int(void)
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	//USART1 TX
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//USART1 RX
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn ;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);	
	USART_Cmd(USART1, ENABLE);
	
}

void SendByte(USART_TypeDef* USARTx,uint8_t byte)
{
	USART_SendData(USARTx,byte);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);	
}

void Serial_SendString(USART_TypeDef* USARTx,char *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		SendByte(USARTx,String[i]);
	
    }
	
}


extern SemaphoreHandle_t UsartMutex;
void Serial_Printf(const char *format, ...)
{
    if (xSemaphoreTake(UsartMutex, portMAX_DELAY) == pdTRUE)
    {
        char buf[100];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        Serial_SendString(USART1,buf);

        xSemaphoreGive(UsartMutex);
    }
}

void USART1_IRQHandler(void)
{
	
    if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET )
	{
		uint16_t data;    
		data=USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_IT_RXNE);
	}
	
}
