#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stm32f10x_spi.h" 		

/*SPI�ӿڶ���-��ͷ****************************/
#define      FLASH_SPIx                        SPI2
#define      FLASH_SPI_APBxClock_FUN          RCC_APB1PeriphClockCmd
#define      FLASH_SPI_CLK                     RCC_APB1Periph_SPI2

//CS(NSS)���� Ƭѡѡ��ͨGPIO����
#define      FLASH_SPI_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CS_CLK                  RCC_APB2Periph_GPIOB	   
#define      FLASH_SPI_CS_PORT                 GPIOB
#define      FLASH_SPI_CS_PIN                  GPIO_Pin_12

//SCK����
#define      FLASH_SPI_SCK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      FLASH_SPI_SCK_CLK                 RCC_APB2Periph_GPIOB   
#define      FLASH_SPI_SCK_PORT                GPIOB  
#define      FLASH_SPI_SCK_PIN                 GPIO_Pin_13
//MISO����
#define      FLASH_SPI_MISO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MISO_CLK                RCC_APB2Periph_GPIOB   
#define      FLASH_SPI_MISO_PORT               GPIOB 
#define      FLASH_SPI_MISO_PIN                GPIO_Pin_14
//MOSI����
#define      FLASH_SPI_MOSI_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MOSI_CLK                RCC_APB2Periph_GPIOB   
#define      FLASH_SPI_MOSI_PORT               GPIOB
#define      FLASH_SPI_MOSI_PIN                GPIO_Pin_15

#define  	SPI_FLASH_CS_LOW()     				GPIO_ResetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )
#define  	SPI_FLASH_CS_HIGH()    				GPIO_SetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )

#define		SPI_FLASH_CS 		PBout(12)		//SPIƬѡ





void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�


#endif

