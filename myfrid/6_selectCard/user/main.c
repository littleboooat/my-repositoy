
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
		//OLED_ShowString(1,1,"          ");
		vTaskDelay(100);
		OLED_ShowString(1,1,"i love you");
		
	}
}

void spi_test(void *para)
{
	int val,size;
	uint8_t result_que;
	uint8_t back_bits;
	uint8_t back_datas[16];
	while(1)
	{   
//		OLED_Clear();
		val=MFR522_ReadReg(0x37);
		OLED_ShowString(3, 1, "          ");
		OLED_ShowHexNum(1, 14, val, 2);//读取到92，则连接成功
		vTaskDelay(1000);
		if(MFRC522_Request(0x26 ,&back_bits)==MI_OK) 
		{
		    result_que=MFRC522_Anticoll(back_datas);
		 
//Serial_Printf("TxControlReg = 0x%02X\r\n", MFR522_ReadReg(TxControlReg));
		
		    if(result_que == MI_OK)
                OLED_ShowString(4, 4, "REQ OK ");
            else if(result_que == MI_NOTAGERR)
               OLED_ShowString(4, 4, "NO TAG ");
            else
                OLED_ShowString(4, 4, "REQ ERR");
			for(int i=3;i>-1;i--)
			{
				OLED_ShowHexNum(2,7-2*i, back_datas[i], 2);
			}
			size=MFRC522_SelectCard(back_datas);
			if(size==0x08)
			OLED_ShowString(3, 4, "success ");	
		    vTaskDelay(500);
		}
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
	MFRC522_Init();
  
	UsartMutex=xSemaphoreCreateMutex();
	I2CMutex=xSemaphoreCreateMutex();
	OLED_Init();//oled的初始化要放在互斥量的后面
	
	xTaskCreate(mytask,"mytask",128,NULL,2,&mytaskhandle);
	xTaskCreate(spi_test,"mytask",128,NULL,2,&spitest_handle);
	vTaskStartScheduler();
	while(1)
	{
	
	}
    return 0;
}
