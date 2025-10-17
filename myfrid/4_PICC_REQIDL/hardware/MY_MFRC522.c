#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include"semphr.h"
#include <stdarg.h>   // 提供 va_list, va_start, va_end
#include <stdio.h>    // 提供 vsnprintf()
#include"MFRC522_DEF.h"
#include"myspi.h"
#include"Delay.h"
void MFRC522_DumpRegs(void)
{
    uint8_t regs[] = {0x37, ModeReg, TxModeReg, RxModeReg,
                      TxControlReg, TxASKReg, RFCfgReg,
                      TModeReg, TPrescalerReg, TReloadRegH, TReloadRegL,CommIEnReg,CommIrqReg,FIFOLevelReg,CommandReg,MFRC522_REG_RX_THRESHOLD};
    const char* names[] = {"Version","Mode","TxMode","RxMode",
                           "TxCtrl","TxASK","RFCfg",
                           "TMode","TPrescale","TReloadH","TReloadL","CommIEnReg","CommIrqReg","FIFOLevelReg","CommandReg","MFRC522_REG_RX_THRESHOLD"};
    for(int i=0;i<sizeof(regs);i++){
        Serial_Printf("%s = 0x%02X\r\n", names[i], MFR522_ReadReg(regs[i]));
    }
}
void MFRC522_ClearBitMask(uint8_t addr,uint8_t mask)
{	
	uint8_t val;
	val=MFR522_ReadReg(addr);
	MFR522_WriteReg(addr,val&(~mask));
}

void MFRC522_SetBitMask(uint8_t addr,uint8_t mask)
{	
	uint8_t val;
	val=MFR522_ReadReg(addr);
	MFR522_WriteReg(addr,val|mask);
}

void MFRC522_Init(void)
{
	 SPI_int();
	 MFR522_WriteReg(CommandReg, PCD_RESETPHASE);   // 软件复位

    /* —— 定时器 约250 ms —— (D3e+1)*(0x32+1)/6.78 */
    MFR522_WriteReg(TModeReg,       0x8D);//最高位启动定时器，低四位为分频器的高四位
    MFR522_WriteReg(TPrescalerReg,  0x3E);//分频器的低八位
    MFR522_WriteReg(TReloadRegL,    0x30);
    MFR522_WriteReg(TReloadRegH,    0);
 
    /* —— 基本通信参数 —— */
// ModeReg寄存器：配置MFRC522的工作模式
// 这里设置为0x3D，表示启用CRC校验（0x6363）以确保数据传输的完整性
    MFR522_WriteReg(ModeReg,        0x3D);         // CRC 初值 0x6363, 启用 TxWaitRF。
//    MFR522_WriteReg(TxModeReg,      0x00);         // 106 kbps, 无调制倒置
//    MFR522_WriteReg(RxModeReg,      0x00);         // 106 kbps, 打开 RxCRC
//逻辑0：无载波输出（0%幅度）
//逻辑1：全幅度载波输出 设置为100% ASK意味着调制深度达到最大值，射频信号幅值在逻辑1时完全开启，逻辑0时完全关闭。
    MFR522_WriteReg(TxASKReg ,      0x40);  
//    MFR522_WriteReg(CommandReg, 0x00);	
    MFR522_WriteReg(RFCfgReg,       0x7f);         // RxGain 最大 (可再微调)
    MFR522_WriteReg(MFRC522_REG_RX_THRESHOLD, 0x84);
    /* —— 打开天线 —— */
    MFRC522_SetBitMask(TxControlReg, 0x03);        // TX1 & TX2 打开

    /* —— FIFO 和 BitFraming —— */
//    MFR522_WriteReg(FIFOLevelReg, 0x80);  // 清空 FIFO
//    MFR522_WriteReg(BitFramingReg, 0x07);  // 设置为 7 位数据帧
	
//	vTaskDelay(pdMS_TO_TICKS(10));        // **新增**：≥5 ms 让卡上电完成
//	  MFR522_WriteReg(TReloadRegL, 0xA0);   // 0x0A0 = 160  → 1 600 µs
//    MFR522_WriteReg(TReloadRegH, 0x03);   // (0x03A0 ≈ 1.6 ms)
}







uint8_t MFRC522_ToCard(uint8_t Command,uint8_t *senddata,uint8_t dataNum,uint8_t *back_datas,uint16_t *backlen)
{
	uint8_t irqen,waitIrq,i,j,irqstate,status;
	uint16_t maxwait;
	uint8_t n,bits_LastByte;
	//memset(back_datas,0,16);
   
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
//Serial_Printf("waitIrq=%x\r\n",waitIrq);	
/*开启相应中断，清中断标志位，清空 FIFO 指针*/
	MFR522_WriteReg(CommIEnReg,irqen|0x80);
	MFRC522_ClearBitMask(CommIrqReg,0x80);	
	MFRC522_SetBitMask(FIFOLevelReg,0x80);
	MFR522_WriteReg(CommandReg,PCD_IDLE);	
//	MFRC522_DumpRegs();
	
/*写数据进buf并开启发送*/
	for(i=0;i<dataNum;i++)
	{
		MFR522_WriteReg(FIFODataReg ,senddata[i]);
	}	

//	Serial_Printf("FIFODataReg2=%x\r\n",MFR522_ReadReg(FIFODataReg));
	MFR522_WriteReg(CommandReg,Command);
//Serial_Printf("PCD_TRANSCEIVE=%x\r\n",MFR522_ReadReg(CommandReg));
/*只有PCD_TRANSCEIVE才需要开启发送，PCD_AUTHENT不需要；*/
	
	if(Command==PCD_TRANSCEIVE) 
		MFRC522_SetBitMask(BitFramingReg,0x80);		
    
//Serial_Printf("BitFramingReg=0x%02x\r\n",MFR522_ReadReg(BitFramingReg));	
/*发送后等待中断标志状态，当有错误是退出接收*/	
	maxwait=2000;
	do
	{
		irqstate=MFR522_ReadReg(CommIrqReg);
		maxwait--;
		}while((maxwait!=0) && !(irqstate&waitIrq) && !(irqstate&0x01));//!(irqstate&waitIrq)表示b5b4只要有一个为时跳出循环
		
//Serial_Printf("irqstate=0x%x,MFR522_ReadReg(ErrorReg)=0x%02x\r\n,maxwait=%d",irqstate,MFR522_ReadReg(ErrorReg),maxwait);
//vTaskDelay(100)	;	

		
		
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
Serial_Printf("n=%d\r\n",n);	
		        bits_LastByte=MFR522_ReadReg(ControlReg)&0x07;//最后三位表示最后一字节的有效位数，0表示一整节有效
				if(bits_LastByte!=0)
                					
				   *backlen=(n-1)*8+bits_LastByte;
				else *backlen=n*8;	
		
Serial_Printf("backlen=%d\r\n",*backlen);				
 /*读取接收的数据*/				
				if(n==0) n=1;
				if(n>16) n=16;
				for(j=0;j<n;j++)				
				{
					back_datas[j]=MFR522_ReadReg(FIFODataReg);
Serial_Printf("backdates=%x\r\n",back_datas[j]);	
				
				}
				
				
			}
		}
		else 
			status=MI_ERR;//操作失败
	
	
		
	}
	       
//  MFRC522_SetBitMask(ControlReg,0x80);// stop timer now
//  MFR522_WriteReg(CommandReg,PCD_IDLE);  
        return status;  		
			
  
}	

uint8_t MFRC522_Request(uint8_t reg_mode,uint8_t * TagType)
{
	uint8_t status;
    uint16_t backlen=0;
	//uint8_t senddata[1];
	//uint8_t back_datas[16]={0};
	
	MFRC522_ClearBitMask(Status2Reg ,0x08);//取消加密	
    MFR522_WriteReg(BitFramingReg, 0x07);   // 再写 TxLastBits = 7
		
//	MFRC522_ClearBitMask(TxControlReg, 0x03); // 清除 Tx1 和 Tx2 开关位
//    MFRC522_SetBitMask(TxControlReg, 0x03);   // 设置 Tx1 和 Tx2 为1，打开天线
	
	
	TagType[0]=reg_mode;
	status= MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1,TagType, &backlen);

   Serial_Printf("status = %d\r\n",backlen);      

	if (status == MI_OK && backlen == 16)
    {
		TagType[0] = backlen;
    } 
    else status = MI_ERR;
    
	return status;
}

