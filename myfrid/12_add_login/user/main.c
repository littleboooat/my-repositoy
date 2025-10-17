
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "OLED.h"
#include "myspi.h"
#include "MySPI2.h"
#include "MY_MFRC522.h"
#include "MFRC522_DEF.h"
#include "myrtc.h"
#include "W25Q64.h"
#include "esp8266.h"
#include "onenet.h"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
uint8_t MID;							//定义用于存放MID号的变量
uint16_t DID;							//定义用于存放DID号的变量
TaskHandle_t mytaskhandle;
TaskHandle_t spitest_handle;
TaskHandle_t MYRTC_handle;
SemaphoreHandle_t UsartMutex;
SemaphoreHandle_t I2CMutex;

void mytask(void *para)
{
	
//	whitelistitem item;
//	uint8_t add_state;
//    whitelistitem newitem={{0xE4,0x13,0x34,0x2D},"david",2};
//    whitelistitem a={{0x34,0xCF,0X38,0X2D},"ryan",1};	
//	
//	add_state =add_whitelist(&newitem);
//Serial_Printf("add_state =%d\r\n",add_state);	
//	W25Q64_ReadData(0x000000, (uint8_t*)&item, sizeof(whitelistitem));
//Serial_Printf("name=%s\r\n",item.NAME);
//	
//    add_whitelist(&a);	
//	W25Q64_ReadData(sizeof(whitelistitem), (uint8_t*)&item, sizeof(whitelistitem));
//Serial_Printf("name=%s\r\n",item.NAME);
	esp8266_init();
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	vTaskDelay(500);
	UsartPrintf(USART1, "Connect MQTT Server Success\r\n");
	while(OneNet_DevLink())			//接入OneNET
	vTaskDelay(500);
	uint8_t timeCount=0;
	while(1)
	{   
//		GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
//		vTaskDelay (500);
//		GPIO_SetBits(GPIOC, GPIO_Pin_13);	
//		vTaskDelay (500);		
//		//OLED_ShowString(1,1,"          ");
//		vTaskDelay(100);
//		OLED_ShowString(4,1,"i love you");
	
	if(++timeCount >= 10)									//发送间隔5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			 esp8266_clear();
		}

	vTaskDelay(1000);	
	}
}

void spi_test(void *para)
{
	uint8_t val,size, status_auth,status_write,status_read;
	uint8_t key[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t result_que;
	uint8_t blockAddr=4;
	uint8_t back_bits;
	uint8_t back_datas[16];
	uint8_t buffer[18]={0};
	uint32_t item_addr;
	whitelistitem item;
	while(1)
	{   
		uint8_t writeData[18] = {
    0x12, 0x34, 0x56, 0x78,
    0x9A, 0xBC, 0xDE, 0xF0,
    0x00, 0x11, 0x22, 0x33,
    0x44, 0x55, 0x66, 0x77
};
//		OLED_Clear();
		val=MFR522_ReadReg(0x37);

		OLED_ShowHexNum(2, 14, val, 2);//读取到92，则连接成功
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
			for(int i=0;i<4;i++)
			{
				OLED_ShowHexNum(2,1+2*i, back_datas[i], 2);
			}
			
			item_addr=inquire_whitelist( back_datas);
Serial_Printf("item_addr =%d\r\n", item_addr);
			W25Q64_ReadData(item_addr, (uint8_t*)&item, sizeof(whitelistitem));
			OLED_ShowString(3, 2, item.NAME);
			vTaskDelay(2000);
			OLED_ShowString(3, 1, "       ");
Serial_Printf("name=%s\r\n",item.NAME);
//			
//			size=MFRC522_SelectCard(back_datas);
//			if(size==0x08)
//			OLED_ShowString(3, 1, "select success ");	
//			vTaskDelay(1000);
//			OLED_ShowString(3, 1, "                  ");	
			
//		    status_auth=MFRC522_Auth(authModeA ,7,key,back_datas);
//			if(status_auth==MI_OK)
//				OLED_ShowString(3, 1, "auth success ");					
//            vTaskDelay(1000);
//			OLED_ShowString(3, 1, "                   ");
//			status_write=MFRC522_WriteBlock(6, writeData);
//			if(status_write==MI_OK)
//				OLED_ShowString(3, 1, "write success ");
//			vTaskDelay(1000);
//			status_read =MFRC522_Read(6, buffer);
//			if(status_read==MI_OK)
//			{	
//				OLED_ShowString(3, 1, "                   ");
//				for(uint8_t i=0;i<16;i++)
//				   { OLED_ShowHexNum(3,1, buffer[i], 2);
//				    vTaskDelay(1000);}
//			}
			
		}
	}
}
 
void myRTC(void *para)
{
	while(1)
	{
		
		myRTCreadtime();
        OLED_ShowNum(1,1,mytime[0],4);
		OLED_ShowString(1, 5, "-");
		OLED_ShowNum(1,6,mytime[1],2);
		OLED_ShowString(1, 8, "-");
		OLED_ShowNum(1,9,mytime[2],2);
		
		OLED_ShowNum(2,1,mytime[3],2);
		OLED_ShowString(2, 3, ":");
		OLED_ShowNum(2,4,mytime[4],2);
		OLED_ShowString(2,6 , ":");
		OLED_ShowNum(2,7,mytime[5],2);
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
	 GPIO_SetBits(GPIOC, GPIO_Pin_13);
//	 ini_usart();
//     Usart2_Init();
	usart_int();
	usart2_int();
//	MFRC522_Init();
//    
	UsartMutex=xSemaphoreCreateMutex();
//	I2CMutex=xSemaphoreCreateMutex();
//	OLED_Init();//oled的初始化要放在互斥量的后面
//	OLED_Clear();
//	W25Q64_Init();						//W25Q64初始化
//	myRTC_Init();
// ESP8266_Init();
 
//	W25Q64_SectorErase(0x000000);	
	/*显示静态字符串*/
//	OLED_ShowString(1, 1, "MID:   DID:");
	
	/*显示ID号*/
//	W25Q64_ReadID(&MID, &DID);			//获取W25Q64的ID号
//	OLED_ShowHexNum(1, 5, MID, 2);		//显示MID
//	OLED_ShowHexNum(1, 12, DID, 4);		//显示DID
//Serial_Printf("DID=%x\r\n",DID);
	xTaskCreate(mytask,"mytask",2048,NULL,2,&mytaskhandle);
//	xTaskCreate(spi_test,"mytask",128,NULL,2,&spitest_handle);
//	xTaskCreate(myRTC,"myRTC",128,NULL,2,&MYRTC_handle);
	vTaskStartScheduler();
	while(1)
	{
	
	}
    return 0;
}
