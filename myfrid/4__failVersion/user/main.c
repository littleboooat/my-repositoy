
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "OLED.h"
#include "myspi.h"
#include "MY_MFRC522.h"
#include"MFRC522_DEF.h"
#define MI_OK           0       // 操作成功
#define MI_NOTAGERR     1       // 没有卡片响应 
#define MI_ERR          2       // 操作失败
TaskHandle_t mytaskhandle;
TaskHandle_t spitest_handle;
SemaphoreHandle_t UsartMutex;
SemaphoreHandle_t I2CMutex;

void mytask(void *para)
{
	while(1)
	{   
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
		GPIO_SetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
		//Serial_Printf("i love you\r\n");
		//OLED_ShowString(1,1,"          ");
		vTaskDelay(100);
		OLED_ShowString(1,1,"i love you");
		
	}
}

void spi_test(void *para)
{
	int val;
	uint8_t result_que;
	uint8_t back_bits;
	 MFR522_WriteReg(CommandReg, PCD_RESETPHASE);   // 软件复位

    /* —— 定时器 300 µs —— */
    MFR522_WriteReg(TModeReg,       0x8D);
    MFR522_WriteReg(TPrescalerReg,  0x3E);
    MFR522_WriteReg(TReloadRegL,    30);
    MFR522_WriteReg(TReloadRegH,    0);

    /* —— 基本通信参数 —— */
    MFR522_WriteReg(ModeReg,        0x3D);         // CRC 初值 0x6363, 启用 TxWaitRF
    MFR522_WriteReg(TxModeReg,      0x00);         // 106 kbps, 无调制倒置
    MFR522_WriteReg(RxModeReg,      0x00);         // 106 kbps, 打开 RxCRC
    MFR522_WriteReg(TxASKReg ,      0x40);         // 自动 100% ASK
    MFR522_WriteReg(RFCfgReg,       0x70);         // RxGain 最大 (可再微调)

    /* —— 打开天线 —— */
    MFRC522_SetBitMask(TxControlReg, 0x03);        // TX1 & TX2 打开

    /* —— FIFO 和 BitFraming —— */
    MFR522_WriteReg(FIFOLevelReg, 0x80);  // 清空 FIFO
    MFR522_WriteReg(BitFramingReg, 0x07);  // 设置为 7 位数据帧
	
	vTaskDelay(pdMS_TO_TICKS(10));        // **新增**：≥5 ms 让卡上电完成
	  MFR522_WriteReg(TReloadRegL, 0xA0);   // 0x0A0 = 160  → 1 600 µs
    MFR522_WriteReg(TReloadRegH, 0x03);   // (0x03A0 ≈ 1.6 ms)
	while(1)
	{   
		
		val=MFR522_ReadReg(0x37);
		OLED_ShowHexNum(3, 1, val, 2);//读取到92，则连接成功
		
		result_que=MFRC522_Request(0x52 ,&back_bits);//寻卡（未休眠）
        Serial_Printf("TxControlReg = 0x%02X\r\n", MFR522_ReadReg(TxControlReg));
        	
		if(result_que == MI_OK)
            OLED_ShowString(4, 4, "REQ OK ");
        else if(result_que == MI_NOTAGERR)
            OLED_ShowString(4, 4, "NO TAG ");
        else
            OLED_ShowString(4, 4, "REQ ERR");
		vTaskDelay(500);
		
	}
}
 

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
	
	usart_int();
	SPI_int();
  

    

/* ------- 2. 把接收窗口放宽到 1.5 ms ------- */
  
/* 其余 TModeReg / TPrescalerReg 保持 0x8D / 0x3E 不变 */
	UsartMutex=xSemaphoreCreateMutex();
	I2CMutex=xSemaphoreCreateMutex();
	OLED_Init();//oled的初始化要放在互斥量的后面
	
	//xTaskCreate(mytask,"mytask",128,NULL,2,&mytaskhandle);
	xTaskCreate(spi_test,"mytask",128,NULL,2,&spitest_handle);
	vTaskStartScheduler();
	while(1)
	{
								
	
	}
    return 0;
}
