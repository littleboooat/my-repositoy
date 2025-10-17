
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


TaskHandle_t xHardWareInitTask;
TaskHandle_t xListTask;
TaskHandle_t xCardTask;
TaskHandle_t xTimeTask;
TaskHandle_t xUploadTask;
SemaphoreHandle_t UsartMutex;
SemaphoreHandle_t I2CMutex;
QueueHandle_t name_upload;  // 队列句柄

char pcWriteBuffer[200];
char pc1WriteBuffer[200];
//==========================================================
//	任务名称：TASK1_whitelist（w25q64）
//
//	函数功能：添加白名单，打印白名单
//
//	入口参数：	无
//
//	说明：添加一条白名单后删除，以免重复添加，擦除函数保持注释状态。
//       当前只有w25q64的第一页可存储白名单即可以存储16条
//==========================================================
void Task1_Whitelist(void *para)
{
	W25Q64_Init();//W25Q64初始化
	whitelistitem item;
	uint8_t add_state;
//	W25Q64_SectorErase(0x000000);
	
//以下为模板用完即删
//	  whitelistitem newitem={{0xE4,0x13,0x34,0x2D},"david",2};
//	  whitelistitem a={{0x34,0xCF,0X38,0X2D},"ryan",1};	
//    add_state =add_whitelist(&newitem);
//	  Serial_Printf("add_state =%s\r\n",add_state==1?"fail":"success");
//	  add_whitelist(& a);

//	whitelistitem a={{0xff,0xff,0Xff,0Xff},"ffff",1};	
//	add_state=add_whitelist(&a);
//	Serial_Printf("add_state =%s\r\n",add_state==1?"fail":"success");
	  
//1.打印存储的总条数，以及各条详细内容。
//	print_all();
//1end==========================================================
	while(1)
	{   

	}
}

//==========================================================
//	任务名称：Task2_CardVerification（mfrc522,spi通信）
//
//	函数功能：打卡显示uid，姓名并上传打卡数据给队列。
//
//	入口参数：	无
//
//	说明：刷卡并显示uid，姓名
//==========================================================
void Task2_CardVerification(void *para)
{
	uint8_t val,size, status_auth,status_write,status_read;
	uint8_t key[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t result_que;
	uint8_t blockAddr=4;
	uint8_t back_bits;
	uint8_t back_datas[16];
	uint8_t buffer[18]={0};
	uint32_t item_addr;
	whitelistitem now_item;
	uint8_t writeData[18] = {
								0x12, 0x34, 0x56, 0x78,
								0x9A, 0xBC, 0xDE, 0xF0,
								0x00, 0x11, 0x22, 0x33,
								0x44, 0x55, 0x66, 0x77
                            };
	MFRC522_Init();//包括spi1初始化，frid通信
    gpio_PC13_Init();							
// 1.验证stm32和mfrc522的通信,读取到92，则通信成功。
	val=MFR522_ReadReg(0x37);
	OLED_ShowHexNum(2, 14, val, 2);	
//1end==========================================================
							
	while(1)
	{   
//2.寻卡（未休眠）并读取和显示uid	
		if(MFRC522_Request(0x26 ,&back_bits)==MI_OK) 
		{
		    result_que=MFRC522_Anticoll(back_datas);
		 
//Serial_Printf("TxControlReg = 0x%02X\r\n", MFR522_ReadReg(TxControlReg));
		
		    if(result_que == MI_OK)
                OLED_ShowString(3, 1, "REQ OK ");
            else if(result_que == MI_NOTAGERR)
               OLED_ShowString(3, 1, "NO TAG ");
            else
                OLED_ShowString(3, 1, "REQ ERR");

			for(int i=0;i<4;i++)
			{
				OLED_ShowHexNum(2,1+2*i, back_datas[i], 2);
			}
//2end==========================================================
			
//3.查找白名单，若找到则oled显示uid、姓名且灯亮，否则显示error。			
			item_addr=inquire_whitelist( back_datas);
//Serial_Printf("item_addr =%d\r\n", item_addr);
			if(item_addr!=1)
			{
			W25Q64_ReadData(item_addr, (uint8_t*)&now_item, sizeof(whitelistitem));
			OLED_ShowString(3, 8, now_item.NAME);
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			vTaskDelay(2000);
			GPIO_SetBits(GPIOC, GPIO_Pin_13);	
			OLED_ShowString(3, 1, "               ");
			
Serial_Printf("name=%s\r\n",now_item.NAME);
			xQueueSend(name_upload, &now_item, 0);
			}
			else OLED_ShowString(3, 2, "error");
//3end==========================================================
//选卡			
//			size=MFRC522_SelectCard(back_datas);
//			if(size==0x08)
//			OLED_ShowString(3, 1, "select success ");	
//			vTaskDelay(1000);
//			OLED_ShowString(3, 1, "               ");	
//认证			
//		    status_auth=MFRC522_Auth(authModeA ,7,key,back_datas);
//			if(status_auth==MI_OK)
//				OLED_ShowString(3, 1, "auth success ");					
//            vTaskDelay(1000);
//			OLED_ShowString(3, 1, "                 ");
//写数据			
//			status_write=MFRC522_WriteBlock(6, writeData);
//			if(status_write==MI_OK)
//				OLED_ShowString(3, 1, "write success ");
//			vTaskDelay(1000);
//读数据			
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

//==========================================================
//	任务名称：Task3_Time（rtc）
//
//	函数功能：显示实时时间
//
//	入口参数：	无
//
//	说明：初始时间戳需要自己写，时间戳到秒级
//==========================================================
void Task3_Time(void *para)
{
	
	myRTC_Init();
	while(1)
	{
		
		myRTCreadtime();
//		UsartPrintf(USART1,"%u\r\n", myUnix);
//年月日
        OLED_ShowNum(1,1,mytime[0],4);
		OLED_ShowString(1, 5, "-");
		OLED_ShowNum(1,6,mytime[1],2);
		OLED_ShowString(1, 8, "-");
		OLED_ShowNum(1,9,mytime[2],2);
//时分秒		
		OLED_ShowNum(2,1,mytime[3],2);
		OLED_ShowString(2, 3, ":");
		OLED_ShowNum(2,4,mytime[4],2);
		OLED_ShowString(2,6 , ":");
		OLED_ShowNum(2,7,mytime[5],2);
		
		
	}
	
}

//==========================================================
//	任务名称：Task4_Upload(esp8266，usart2通信)
//
//	函数功能：上传打卡数据至onenet
//
//	入口参数：	无
//
//	说明：
//==========================================================
void Task4_Upload(void *para)
{
	usart2_int();//usart2初始化，用于建立与esp8266的通信		
//实现wifi连接
	esp8266_init();
//用于建立 ESP8266 模块到 OneNET MQTT 服务器的 TCP 网络连接。TCP 网络就是一台设备与另一台设备之间通过“可靠的通道”传输数据的过程。
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	vTaskDelay(500);
	UsartPrintf(USART1, "Connect MQTT Server Success\r\n");
//接入OneNET,用于通过 MQTT 协议向 OneNET 完成设备鉴权和连接。MQTT 协议层是运行在 TCP 网络之上的“应用层协议”，用于设备之间“如何发消息”和“订阅消息”的规范。
	while(OneNet_DevLink())			
	vTaskDelay(500);
	uint8_t timeCount=0;
	while(1)
	{  
		
	if(++timeCount >= 10)									//发送间隔5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			 esp8266_clear();
		}
//==========================================================
//1.打印各个任务栈的使用情况，任务的情况
//    vTaskList(pcWriteBuffer);
//    UsartPrintf(USART1, "%s",pcWriteBuffer);
//2. 查看 CPU 占用时间		
//    vTaskGetRunTimeStats(pc1WriteBuffer);  
//	UsartPrintf(USART1, "%s",pc1WriteBuffer);
//==========================================================
			vTaskDelay(1000);	//稍微大一些，上传太快会掉线
	}
}

int main(void)
{	


	UsartMutex=xSemaphoreCreateMutex();//保证一个字符打印完全
	I2CMutex=xSemaphoreCreateMutex();//保证一个字符打印完全
	name_upload=xQueueCreate( 1,sizeof(whitelistitem));
	
	usart_int();//usart1初始化，用于debug		
	OLED_Init();//oled的初始化要放在互斥量的后面
	OLED_Clear();
//c8t6,rom=64kb,ram=20kb,编译完成后底部的rw+zi=ram(已使用)。code+ro=rom
//当heap4分配太多，会导致全局变量定义不成功。因为ram不够
//总共有configTOTAL_HEAP_SIZE( ( size_t ) ( 10 * 1024 ) )。“任务应至少保留 20% 的栈空间 以防止突发递归、函数调用、局部数组暴增。或者余量大于100”	
//四个任务一共分配了（60+150+96+800）*4=	1154.而一共有17*1024,所以可以适当分配少一些
	xTaskCreate(Task1_Whitelist,"MYwhitelist",60,NULL,2,&xListTask);//大概使用10word。60表示60*4字节
	xTaskCreate(Task2_CardVerification,"MYlist",150,NULL,2,&xCardTask);//大概使用98word
	xTaskCreate(Task3_Time,"MYtime",96,NULL,2,&xTimeTask);//大概使用46word
	xTaskCreate(Task4_Upload,"MYupload",800,NULL,2,&xUploadTask);//大概使用630word,打算剩余25%，则630*1.25约等于800

	vTaskStartScheduler();
	while(1)
	{
	
	}
    return 0;
}
