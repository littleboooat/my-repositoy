#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include"semphr.h"
#include <stdarg.h>   // 提供 va_list, va_start, va_end
#include <stdio.h>    // 提供 vsnprintf()
#include"MFRC522_DEF.h"
#include"myspi.h"





void MFRC522_ClearBitMask(uint8_t addr,uint8_t mask)
{	
	uint8_t val;
	val=MFR522_ReadReg(addr);
	MFR522_WriteReg(addr,val&~mask);
}

void MFRC522_SetBitMask(uint8_t addr,uint8_t mask)
{	
	uint8_t val;
	val=MFR522_ReadReg(addr);
	MFR522_WriteReg(addr,val|mask);
}


uint8_t MFRC522_ToCard(uint8_t Command,uint8_t *senddata,uint8_t dataNum,uint8_t *back_datas,uint8_t *backlen)
{
	uint8_t irqen,waitIrq,i,j,maxwait,irqstate,status;
	 uint8_t n,bits_LastByte;
/*判定命令类型，指定相应的中断位和发送后等待的中断位*/
	if(Command==PCD_AUTHENT)
	{
		irqen=0x12;
		waitIrq=0x10;
	}	

	if(Command==PCD_TRANSCEIVE )
	{
		irqen=0x77;
		waitIrq=0x30;
	}	
/*开启相应中断，清中断标志位，清空 FIFO 指针*/
	MFR522_WriteReg(CommIEnReg,irqen|0x80);
	MFRC522_ClearBitMask(CommIrqReg,0x80);	
	MFRC522_SetBitMask(FIFOLevelReg,0x80);
	MFR522_WriteReg(CommandReg,PCD_IDLE);	
/*写数据进buf并开启发送*/
	for(i=0;i<dataNum;i++)
	{
		MFR522_WriteReg(FIFODataReg ,senddata[i]);
	}	
Serial_Printf("FIFODataReg=%x=\r\n",MFR522_ReadReg(FIFODataReg));
	MFR522_WriteReg(CommandReg,Command);
/*只有PCD_TRANSCEIVE才需要开启发送，PCD_AUTHENT不需要；*/
	if(Command==PCD_TRANSCEIVE) 
	
		MFR522_WriteReg(BitFramingReg,0x87);
	MFR522_WriteReg(BitFramingReg,0x00);//发七位
Serial_Printf("BitFramingReg=0x%02x\r\n",MFR522_ReadReg(BitFramingReg));	
/*发送后等待中断标志状态，当有错误是退出接收*/	
	maxwait=40000;
	do
	{
		irqstate=MFR522_ReadReg(CommIrqReg);
		maxwait--;
		}while((maxwait!=0) && !(irqstate&waitIrq) && !(irqstate&0x01));//!(irqstate&waitIrq)表示b5b4只要有一个为时跳出循环
	
Serial_Printf("irqstate=0x%x,MFR522_ReadReg(ErrorReg)=0x%x\r\n",irqstate,MFR522_ReadReg(ErrorReg));

/*未超时且无错误则成功*/
	if(maxwait!=0)
	{
		if(!(MFR522_ReadReg(ErrorReg)&0x1b))
		{
			 status = MI_OK;
            if (irqstate & irqen & 0x01) status = MI_NOTAGERR;

			if(Command==PCD_TRANSCEIVE )//只有收发命令才接收数据
			{
 /*读取接受的字节，计算接收的总位数*/	
               				
				n=MFR522_ReadReg(FIFOLevelReg);
				bits_LastByte=MFR522_ReadReg(ControlReg)&0x07;//最后三位表示最后一字节的有效位数，0表示一整节有效
				if(status == MI_OK)
                {					
					if(bits_LastByte) *backlen=(n-1)*8+bits_LastByte;
					else *backlen=n*8;	
				}
 /*读取接收的数据*/				
				if(n==0) n=1;
				if(n>16) n=16;
				for(j=0;j<n;j++)				
					back_datas[j]=MFR522_ReadReg(FIFODataReg);
				
			}
		}
		else status=MI_ERR;//操作失败
	
	
		
	}
	   
        return status;  		
}			
      
//  MFRC522_SetBitMask(ControlReg,0x80);// stop timer now
//    MFR522_WriteReg(CommandReg,PCD_IDLE);    
	

uint8_t MFRC522_Request(uint8_t reg_mode,uint8_t *back_bits)
{
	uint8_t status,backlen;
	uint8_t senddata[1];
	uint8_t back_datas[16];
	
	MFRC522_ClearBitMask(Status2Reg ,0x08);//取消加密
	MFRC522_SetBitMask(BitFramingReg,0x07);//发七位
		
//	MFRC522_ClearBitMask(TxControlReg, 0x03); // 清除 Tx1 和 Tx2 开关位
    MFRC522_SetBitMask(TxControlReg, 0x03);   // 设置 Tx1 和 Tx2 为1，打开天线
	
	
	senddata[0]=reg_mode;
	status= MFRC522_ToCard(PCD_TRANSCEIVE, senddata, 1, back_datas, &backlen);
	
	  Serial_Printf("backlen = %d\r\n", backlen);
       for (int i = 0; i <2; i++) {
    Serial_Printf("back_datas=0x%02X\r\n ", back_datas[i]);
}
	if (status == MI_OK && backlen == 0x10)
    {
		*back_bits = backlen;
    } 
    else {
    status = MI_ERR;
    }
	return status;
}

void MFRC522_DumpRegs(void)
{
    uint8_t regs[] = {0x37, ModeReg, TxModeReg, RxModeReg,
                      TxControlReg, TxASKReg, RFCfgReg,
                      TModeReg, TPrescalerReg, TReloadRegH, TReloadRegL};
    const char* names[] = {"Version","Mode","TxMode","RxMode",
                           "TxCtrl","TxASK","RFCfg",
                           "TMode","TPrescale","TReloadH","TReloadL"};
    for(int i=0;i<sizeof(regs);i++){
        Serial_Printf("%s = 0x%02X\r\n", names[i], MFR522_ReadReg(regs[i]));
    }
}