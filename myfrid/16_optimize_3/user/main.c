
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



TaskHandle_t xStatsTask;
TaskHandle_t xHardWareInitTask;
TaskHandle_t xListTask;
TaskHandle_t xCardTask;
TaskHandle_t xTimeTask;
TaskHandle_t xUploadTask;
TaskHandle_t xListTask;
SemaphoreHandle_t UsartMutex;
SemaphoreHandle_t I2CMutex;
QueueHandle_t name_upload;  // ���о��

//void vApplicationIdleHook( void )
//{

//    char pcWriteBuffer[200];
//	char runTimeStats[512];
//	
//	 

//1.��ӡ��������ջ��ʹ���������������
//    vTaskList(pcWriteBuffer);
//    UsartPrintf(USART1, "%s",pcWriteBuffer);
//2. �鿴 CPU ռ��ʱ��		  
//	vTaskGetRunTimeStats(runTimeStats);
//	UsartPrintf(USART1, "\r\n%s\r\n", runTimeStats);
//  // ? ÿ 2 ���һ��


//}

void Task0_PrintStats(void *para)
{
    char pcWriteBuffer[200];
	char runTimeStats[512];
//	uint8_t a[2]={0xCE ,0xD2};
	
	while(1)
	{   
//������vscode��utf8תΪgb2312�ſ���ʹ�ã���Ϊ utf8����������ռ���ֽڣ�gb2312��2�ֽڡ��ҵ�hzk16�ֿ��Ǹ���gb2312���ġ�
//	OLED_ShowChinese(1, 1, "��");	
//    OLED_ShowChineseString(2,1,"һ�����������߰�");
//	OLED_ShowMixedString(3,1,"nihaoƤ");
////1.��ӡ��������ջ��ʹ���������������
      vTaskList(pcWriteBuffer);
      UsartPrintf(USART1, "%s",pcWriteBuffer);
////2. �鿴 CPU ռ��ʱ��		  
//	  vTaskGetRunTimeStats(runTimeStats);
//	  UsartPrintf(USART1, "\r\n%s\r\n", runTimeStats);
//      vTaskDelay(2000);  // ? ÿ 2 ���һ��
  
	}

}


//==========================================================
//	�������ƣ�TASK1_whitelist��w25q64��
//
//	�������ܣ���Ӱ���������ӡ������
//
//	��ڲ�����	��
//
//	˵�������һ����������ɾ���������ظ���ӣ�������������ע��״̬��
//       ��ǰֻ��w25q64�ĵ�һҳ�ɴ洢�����������Դ洢16��
//==========================================================

void Task1_Whitelist(void *para)
{
	
	whitelistitem item;
	uint8_t add_state;
	uint8_t buffer[32];  // �������һ�����ֵĵ�������
	W25Q64_ReadData(0x1000+136928,buffer,32);//136928Ϊ���ҡ���ƫ����
	for(int i=0;i<32;i++)
	{
	Serial_Printf("buffer =%x\r\n",buffer[i]);
	}
//	W25Q64_SectorErase(0x000000);
	
//����Ϊģ�����꼴ɾ
//	  whitelistitem newitem={{0xE4,0x13,0x34,0x2D},"david",2};
//	  whitelistitem a={{0x34,0xCF,0X38,0X2D},"ryan",1};	
//    add_state =add_whitelist(&newitem);
//	  Serial_Printf("add_state =%s\r\n",add_state==1?"fail":"success");
//	  add_whitelist(& a);

//	whitelistitem a={{0xff,0xff,0Xff,0Xff},"ffff",1};	
//	add_state=add_whitelist(&a);
//	Serial_Printf("add_state =%s\r\n",add_state==1?"fail":"success");
	  
//1.��ӡ�洢�����������Լ�������ϸ���ݡ�
//	print_all();
//1end==========================================================

	while(1)
	{   
		vTaskDelay(500);
	}
}

//==========================================================
//	�������ƣ�Task2_CardVerification��mfrc522,spiͨ�ţ�
//
//	�������ܣ�����ʾuid���������ϴ������ݸ����С�
//
//	��ڲ�����	��
//
//	˵����ˢ������ʾuid������
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
	MFRC522_Init();//����spi1��ʼ����fridͨ��
    gpio_PC13_Init();							
// 1.��֤stm32��mfrc522��ͨ��,��ȡ��92����ͨ�ųɹ���
	val=MFR522_ReadReg(0x37);
	OLED_ShowHexNum(2, 14, val, 2);	
//1end==========================================================
							
	while(1)
	{   
		
//2.Ѱ����δ���ߣ�����ȡ����ʾuid	
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
			
//3.���Ұ����������ҵ���oled��ʾuid�������ҵ�����������ʾerror��			
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
			vTaskDelay(500);
Serial_Printf("name=%s\r\n",now_item.NAME);
			xQueueSend(name_upload, &now_item, 0);
			}
			else OLED_ShowString(3, 2, "error");
//3end==========================================================
//ѡ��			
//			size=MFRC522_SelectCard(back_datas);
//			if(size==0x08)
//			OLED_ShowString(3, 1, "select success ");	
//			vTaskDelay(1000);
//			OLED_ShowString(3, 1, "               ");	
//��֤			
//		    status_auth=MFRC522_Auth(authModeA ,7,key,back_datas);
//			if(status_auth==MI_OK)
//				OLED_ShowString(3, 1, "auth success ");					
//            vTaskDelay(1000);
//			OLED_ShowString(3, 1, "                 ");
//д����			
//			status_write=MFRC522_WriteBlock(6, writeData);
//			if(status_write==MI_OK)
//				OLED_ShowString(3, 1, "write success ");
//			vTaskDelay(1000);
//������			
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
//	�������ƣ�Task3_Time��rtc��
//
//	�������ܣ���ʾʵʱʱ��
//
//	��ڲ�����	��
//
//	˵������ʼʱ�����Ҫ�Լ�д��ʱ������뼶
//==========================================================
void Task3_Time(void *para)
{
//	UsartPrintf(USART1,"rrryy");
	myRTC_Init();
	while(1)
	{
		
		myRTCreadtime();
		
//		UsartPrintf(USART1,"%u\r\n", myUnix);
//������
        OLED_ShowNum(1,1,mytime[0],4);
		OLED_ShowString(1, 5, "-");
		OLED_ShowNum(1,6,mytime[1],2);
		OLED_ShowString(1, 8, "-");
		OLED_ShowNum(1,9,mytime[2],2);
//ʱ����		
		OLED_ShowNum(2,1,mytime[3],2);
		OLED_ShowString(2, 3, ":");
		OLED_ShowNum(2,4,mytime[4],2);
		OLED_ShowString(2,6 , ":");
		OLED_ShowNum(2,7,mytime[5],2);
		vTaskDelay(500);
		
	}
	
}

//==========================================================
//	�������ƣ�Task4_Upload(esp8266��usart2ͨ��)
//
//	�������ܣ��ϴ���������onenet
//
//	��ڲ�����	��
//
//	˵����
//==========================================================
void Task4_Upload(void *para)
{
	usart2_int();//usart2��ʼ�������ڽ�����esp8266��ͨ��		
//ʵ��wifi����
	esp8266_init();
//���ڽ��� ESP8266 ģ�鵽 OneNET MQTT �������� TCP �������ӡ�TCP �������һ̨�豸����һ̨�豸֮��ͨ�����ɿ���ͨ�����������ݵĹ��̡�
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	vTaskDelay(500);
	UsartPrintf(USART1, "Connect MQTT Server Success\r\n");
//����OneNET,����ͨ�� MQTT Э���� OneNET ����豸��Ȩ�����ӡ�MQTT Э����������� TCP ����֮�ϵġ�Ӧ�ò�Э�顱�������豸֮�䡰��η���Ϣ���͡�������Ϣ���Ĺ淶��
	while(OneNet_DevLink())			
	vTaskDelay(500);
	uint8_t timeCount=0;
	while(1)
	{  
		
	if(++timeCount >= 10)									//���ͼ��5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//��������
			
			timeCount = 0;
			 esp8266_clear();
		}

			vTaskDelay(1000);	//��΢��һЩ���ϴ�̫������
	}
}
\

int main(void)
{	


	UsartMutex=xSemaphoreCreateMutex();//��֤һ���ַ���ӡ��ȫ
	I2CMutex=xSemaphoreCreateMutex();//��֤һ���ַ���ӡ��ȫ
	name_upload=xQueueCreate( 1,sizeof(whitelistitem));
	
	W25Q64_Init();//W25Q64��ʼ��
	
	usart_int();//usart1��ʼ��������debug		
	OLED_Init();//oled�ĳ�ʼ��Ҫ���ڻ������ĺ���
	OLED_Clear();
//c8t6,rom=64kb,ram=20kb,������ɺ�ײ���rw+zi=ram(��ʹ��)��code+ro=rom
//��heap4����̫�࣬�ᵼ��ȫ�ֱ������岻�ɹ�����Ϊram����
//�ܹ���configTOTAL_HEAP_SIZE( ( size_t ) ( 10 * 1024 ) )��������Ӧ���ٱ��� 20% ��ջ�ռ� �Է�ֹͻ���ݹ顢�������á��ֲ����鱩����������������100��	
//�ĸ�����һ�������ˣ�60+150+96+800��*4=	1154.��һ����17*1024,���Կ����ʵ�������һЩ
//	xTaskCreate(Task0_PrintStats,"MYPrintStats",400,NULL,2,&xStatsTask);//���ʹ��310word��
//	xTaskCreate(Task1_Whitelist,"MYwhitelist",100,NULL,2,&xListTask);//���ʹ��10word��60��ʾ60*4�ֽ�
	xTaskCreate(Task2_CardVerification,"MYcard",150,NULL,2,&xCardTask);//���ʹ��98word
	xTaskCreate(Task3_Time,"MYtime",100,NULL,2,&xTimeTask);//���ʹ��46word
	xTaskCreate(Task4_Upload,"MYupload",800,NULL,2,&xUploadTask);//���ʹ��630word,����ʣ��25%����630*1.25Լ����800

	vTaskStartScheduler();
	while(1)
	{
	
	}
    return 0;
}
