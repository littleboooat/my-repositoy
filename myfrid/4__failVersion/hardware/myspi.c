#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include"semphr.h"
#include <stdarg.h>   // 提供 va_list, va_start, va_end
#include <stdio.h>    // 提供 vsnprintf()
#define SS_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_4)   // 置高电平 
#define SS_LOW()  GPIO_ResetBits(GPIOA, GPIO_Pin_4) // 置低电平 


void SPI_int(void)
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	

	//PA4片选，PA5sclk,PA6 MISO输入，PA7输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//spi结构体
	SPI_InitTypeDef SPI_InitStruct;
	//> SPI 时钟频率最大为 10 MHz,推荐范围：1 MHz ~ 10 MHz,APB272HZ
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16 ;
	//模式0，sclk空闲状态低电平，ss下降沿开始移出
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStruct.SPI_CRCPolynomial=7;
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;//MFR是高位先行
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;	
    SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_Cmd(SPI1,ENABLE);
    SS_HIGH();//ss置1，初始状态

}	

uint16_t SPI_TransimitReceive(uint16_t SDATA)
{
	uint16_t RDATA;
	//SS_LOW();
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1, SDATA);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	RDATA = SPI_I2S_ReceiveData(SPI1);	
	//SS_HIGH();	
	return RDATA;
}

void MFR522_WriteReg(uint8_t addr,uint8_t val)
{
	//读命令最高位为0，最低为不管读写都是0
	SS_LOW();
	SPI_TransimitReceive(addr<<1&0x7E);
	SPI_TransimitReceive(val);	
	SS_HIGH();
}

uint8_t MFR522_ReadReg(uint8_t addr)
{
	uint8_t val;
	SS_LOW();
	//写命令最高位为1
	SPI_TransimitReceive(addr<<1|0x80);
	val=SPI_TransimitReceive(0x00);
	SS_HIGH();
	return val;
}