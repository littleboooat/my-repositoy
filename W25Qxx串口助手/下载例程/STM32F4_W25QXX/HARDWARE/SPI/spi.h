#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stm32f4xx_spi.h"	

/*SPI�ӿڶ���-��ͷ****************************/
#define      FLASH_SPIx                        SPI1
#define      FLASH_SPI_APBxClock_FUN          RCC_AHB1Periph_GPIOB
#define      FLASH_SPI_CLK                     RCC_APB2Periph_SPI1

//CS(NSS)���� Ƭѡѡ��ͨGPIO����
#define      FLASH_SPI_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CS_CLK                  RCC_AHB1Periph_GPIOB    
#define      FLASH_SPI_CS_PORT                 GPIOB
#define      FLASH_SPI_CS_PIN                  GPIO_Pin_14

//SCK����
#define      FLASH_SPI_SCK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      FLASH_SPI_SCK_CLK                 RCC_AHB1Periph_GPIOB   
#define      FLASH_SPI_SCK_PORT                GPIOB   
#define      FLASH_SPI_SCK_PIN                 GPIO_Pin_3
//MISO����
#define      FLASH_SPI_MISO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MISO_CLK                RCC_AHB1Periph_GPIOB    
#define      FLASH_SPI_MISO_PORT               GPIOB 
#define      FLASH_SPI_MISO_PIN                GPIO_Pin_4
//MOSI����
#define      FLASH_SPI_MOSI_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MOSI_CLK                RCC_AHB1Periph_GPIOB    
#define      FLASH_SPI_MOSI_PORT               GPIOB 
#define      FLASH_SPI_MOSI_PIN                GPIO_Pin_5

#define  	SPI_FLASH_CS_LOW()     				GPIO_ResetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )
#define  	SPI_FLASH_CS_HIGH()    				GPIO_SetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )

#define		SPI_FLASH_CS 		PBout(14)		//SPIƬѡ





void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�


#endif

