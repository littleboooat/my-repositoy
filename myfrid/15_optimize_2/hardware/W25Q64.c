#include "stm32f10x.h"                  // Device header
#include "MySPI2.h"
#include "W25Q64_Ins.h"
#include "W25Q64.h"


/**
  * 函    数：W25Q64初始化
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_Init(void)
{
	MySPI_Init();					//先初始化底层的SPI
}

/**
  * 函    数：W25Q64读取ID号
  * 参    数：MID 工厂ID，使用输出参数的形式返回
  * 参    数：DID 设备ID，使用输出参数的形式返回
  * 返 回 值：无
  */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_JEDEC_ID);			//交换发送读取ID的指令
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	//交换接收MID，通过输出参数返回
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	//交换接收DID高8位
	*DID <<= 8;									//高8位移到高位
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);	//或上交换接收DID的低8位，通过输出参数返回
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64写使能
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WriteEnable(void)
{
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);		//交换发送写使能的指令
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64等待忙
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 100000;							//给定超时计数时间
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;								//跳出等待，不等了
		}
	}
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64页编程
  * 参    数：Address 页编程的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray	用于写入数据的数组
  * 参    数：Count 要写入数据的数量，范围：0~256
  * 返 回 值：无
  * 注意事项：写入的地址范围不能跨页
  */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q64_WriteEnable();						//写使能
	
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);		//交换发送页编程的指令
	MySPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		MySPI_SwapByte(DataArray[i]);			//依次在起始地址后写入数据
	}
	MySPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy();							//等待忙
}

/**
  * 函    数：W25Q64扇区擦除（4KB）
  * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
  * 返 回 值：无
  */
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();						//写使能
	
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	MySPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte(Address);					//交换发送地址7~0位
	MySPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy();							//等待忙
}

/**
  * 函    数：W25Q64读取数据
  * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
  * 参    数：Count 要读取数据的数量，范围：0~0x800000
  * 返 回 值：无
  */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_READ_DATA);			//交换发送读取数据的指令
	MySPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
	MySPI_Stop();								//SPI终止
}
#define WHITELIST_BASE_ADDR 0x000000
#include <string.h>
#include "usart.h"
#include "myspi.h"
uint32_t inquire_whitelist(uint8_t* UID)
{
	uint8_t i;
	uint32_t addr; 	
	whitelistitem item;

//Serial_Printf ("uid1:%x,uid2:%x\r\n",UID[0],UID[1]);

	for(i=0;i<(256/sizeof(whitelistitem));i++)
	{
		addr=WHITELIST_BASE_ADDR+i*sizeof(whitelistitem);
		W25Q64_ReadData(addr, (uint8_t*)&item, sizeof(whitelistitem));
//Serial_Printf ("ITEM uid1:%x,uid2:%x",item.UID[0],item.UID[0]);		
		if(item.UID[0]==UID[0]&&item.UID[1]==UID[1]&&item.UID[2]==UID[2]&&item.UID[3]==UID[3])
		{
//Serial_Printf ("ITEM uid1:%x,uid2:%x",item.UID[0],item.UID[1]);	
//Serial_Printf ("item.NAME: %S",item.NAME);		
			return addr;
		}	
			
	}
	return 1;
		
}


uint8_t add_whitelist(whitelistitem* NEWITEM)
{
	uint8_t i;
	uint32_t addr; 	
	whitelistitem item;
	

	if(inquire_whitelist(NEWITEM->UID)!=1) 
	{
		return 1;
	}
	for(i=0;i<(256/sizeof(whitelistitem));i++)
	{
		addr=WHITELIST_BASE_ADDR+i*sizeof(whitelistitem);
		W25Q64_ReadData(addr, (uint8_t*)&item, sizeof(whitelistitem));
		if(item.UID[0]==0xFF&&item.UID[1]==0XFF&&item.UID[2]==0XFF&&item.UID[3]==0XFF)
		{
			W25Q64_PageProgram(addr, (uint8_t*)NEWITEM,sizeof(whitelistitem));
			break;	
		}	
			
	}
	
	return addr;
}
uint8_t  print_all(void)
{
	uint32_t addr; 
    uint8_t NUM_list=0;		
	whitelistitem item;
	for(uint8_t i=0;i<(256/sizeof(whitelistitem));i++)
	{
		addr=WHITELIST_BASE_ADDR+i*sizeof(whitelistitem);
		W25Q64_ReadData(addr, (uint8_t*)&item, sizeof(whitelistitem));
		if(item.UID[0]==0xFF&&item.UID[1]==0XFF&&item.UID[2]==0XFF&&item.UID[3]==0XFF)
		{			
		   NUM_list=i;
		   break;
		}	
			
	}
	Serial_Printf("总条目为：%d条.\r\n",NUM_list);
	for(uint8_t i=0;i<NUM_list;i++)
	{
		addr=WHITELIST_BASE_ADDR+i*sizeof(whitelistitem);
		W25Q64_ReadData(addr, (uint8_t*)&item, sizeof(whitelistitem));
		  
        Serial_Printf ("第%d条:uid:%02x%02x%02x%02x,",i,item.UID[0],item.UID[1],item.UID[2],item.UID[03],item.UID[0]);
		Serial_Printf ("name:%s\r\n",item.NAME);
	}



}