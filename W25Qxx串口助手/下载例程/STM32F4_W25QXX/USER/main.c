#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "spi.h"
#include "w25qxx.h"

//ALIENTEK 探索者STM32F407开发板 实验0
//STM32F4工程模板-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

extern uint32_t data_stat;	//串口接收数据个数
extern uint8_t USART_RX_BUF[RECEIVE_DATA_LEN];
extern uint8_t start_flag;
extern uint8_t data_flag;


int main(void)
{
	u32 i = 0;
	uint32_t addr_start = 0;
	uint32_t file_len = 0;
	uint32_t receive_flag = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 					//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init(168);
	uart_init(115200);													//设置串口波特率为115200
	W25QXX_Init();														//W25QXX初始化
	delay_ms(200);
	
	while(W25QXX_ReadID()!=W25Q128)										//检测不到W25Q64
	{
		printf("W25Q65 Check Failed!\r\n");
	}

	printf("W25Qxx Successful initialization\r\n");						//提示W25Qxx已经初始化成功

	//验证W25Q64单个扇区接收数据是否正确
//	W25QXX_Read(USART_RX_BUF, 46*4096 , 4096); 
//	for(i=0; i<4096; i++)
//	{
//		if(USART_RX_BUF[i] > 0xf)
//			printf("%x ", USART_RX_BUF[i]);
//		else
//			printf("0%x ", USART_RX_BUF[i]);
//	}
//	while(1);

	
	USART_RX_BUF[0] = 0;												//上电清除指令接收缓冲区，防止内部数据不确定造成判断失误
	USART_RX_BUF[1] = 0;
	USART_RX_BUF[10] = 0;		
	while(1) 
	{	
		if(start_flag == 0)
		{
			if(USART_RX_BUF[0] == 0xFE && USART_RX_BUF[1] == 0xFF && USART_RX_BUF[10] == 0xFE)		//等待接指令、写入地址和文件长度
			{
				
				start_flag = 1;				//清空串口接收长度，准备接收数据
				data_stat = 0;				//清空数据接收
				receive_flag = data_stat;	//

				addr_start = 0;	
				addr_start |= USART_RX_BUF[2];
				addr_start |= USART_RX_BUF[3] << 8;
				addr_start |= USART_RX_BUF[4] << 16;
				addr_start |= USART_RX_BUF[5] << 24;		//获取存放数据的起始位置

				file_len = 0;
				file_len |= USART_RX_BUF[6];
				file_len |= USART_RX_BUF[7] << 8;
				file_len |= USART_RX_BUF[8] << 16;
				file_len |= USART_RX_BUF[9] << 24;			//获取存放文件长度
				delay_ms(5);
				
				printf("start---"); 													//串口提示已经收到数据接收命令 准备接收数据
				printf("addr_start=%d file_size=%d", addr_start, file_len); 			//从串口要写入文件的起始地址和要写入文件的大小
				
				printf("Start erasing sectors\r\n"); 		//从串口提示要擦除需要写入的扇区
				for(i=addr_start/RECEIVE_DATA_LEN; i<(file_len+addr_start)/RECEIVE_DATA_LEN + (((file_len+addr_start)%RECEIVE_DATA_LEN)>0?1:0); i++)		//擦除需要些入文件的扇区
				{
					W25QXX_Erase_Sector(i);												//扇区擦除
					printf("Erasing sector %d\r\n", i);
					delay_ms(150);
				}
				printf("Sector erasing is complete\r\n");
				printf("\r\nStart writing data\r\n");
				printf("%s","NA");  													//回应上位机已经接收到接收数据命令，已经准备好等待接收上位机发送的数据
			}

		}
		else if(start_flag == 1)														//开始向W25Qxx中写入数据
		{
			if(data_stat % RECEIVE_DATA_LEN == 0 && receive_flag != data_stat)			//从缓存区中读取接收到文件的数据块，写入W25Qxx中		注释：这里的一个文件块为4096Byte
			{
				W25QXX_Write_NoCheck(USART_RX_BUF, addr_start + data_stat-RECEIVE_DATA_LEN, RECEIVE_DATA_LEN);
				receive_flag = data_stat;
				delay_ms(50);
				printf(" %d%s", data_stat, "A");										//回应上位机已经将缓冲区中的数据块写入W25Qxx中等待接收下一个数据块
			}
			else if(data_stat >= file_len)
			{
				W25QXX_Write_NoCheck(USART_RX_BUF, addr_start + data_stat-(file_len%RECEIVE_DATA_LEN), file_len%RECEIVE_DATA_LEN);			//写入文件最后一个块 该数据块不是完整的数据块小于4096Byte
				printf("\r\n%s", "Write to file successfully-----");
				printf("File Size %d Byte", data_stat);		
				while(1);																//接收数据完成
			}
		}
	}
}



/*
手册中讲解到步骤15的时候的main.c源码如下：
#include "stm32f4xx.h"

//ALIENTEK 探索者STM32F407开发板 实验0
//STM32F4工程模板-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
  
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

int main(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  while(1){
		GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
		Delay(0x7FFFFF);
		GPIO_ResetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
		Delay(0x7FFFFF);
	
	}
}
*/


