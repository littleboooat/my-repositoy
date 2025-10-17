#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include"semphr.h"
#include <string.h>
#include <stdio.h>
#include"usart.h"
#define rev_ok 0
#define rev_wait 1
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"xiaomi\",\"66666663\"\r\n"

char rec_buf[256];
uint8_t cnt=0;
uint8_t precnt=0;

uint8_t ESP8266_WaitRecive(void)
{
	if(cnt==0) //表示没有数据
	{
		return rev_wait;
	}		
	if(cnt==precnt) 
	{
		cnt=0;
		return rev_ok;//表示已经接收完毕，因为cnt值不变
	}
	precnt=cnt;
	return rev_wait;
	
      		
}

void  esp8266_clear(void)
{
  memset(rec_buf,0,sizeof(rec_buf));
	cnt=0;
}

uint8_t ESP8266_SendCmd(char* cmd,char* res)
{
	//Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	Serial_SendString(USART2,cmd);
	unsigned char timeOut = 200;
	while(timeOut--)
	{
		if(ESP8266_WaitRecive()==rev_ok)
		{
			if(strstr((const char *)rec_buf,res)!=NULL)
			{
				esp8266_clear();
				return 0;
			}
		}
		vTaskDelay(10);//整个 while 循环最多持续 200 * 10ms = 2000ms = 2秒。如果没有这句，可能几毫秒就跑完，这时却还没收到数据
	}	
	return 1;
}

//void ESP8266_SendData(unsigned char *data, unsigned short len)
//{

//	char cmdBuf[32];
//	
//	esp8266_clear();								//去酄曓撍曐赫?
//	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//注虓募庐
//	if(!ESP8266_SendCmd(cmdBuf, ">"))				//藭战n>o时酄売斪⑻嵥竭?
//	{
//		 Serial_SendString(USART2,data);		//注虓狮迅l迵全瘸私邼
//	}

//}
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	esp8266_clear();								//去酄曓撍曐赫?
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//将字符串 "AT+CIPSEND=数字\r\n" 写入到 cmdBuf 中，数字 的位置会被变量 len 的值替代。
//Serial_Printf("cmdBuf=%s",cmdBuf);
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//藭战n>o时酄売斪⑻嵥竭?
	{
		Usart_SendString(USART2, data, len);		//注虓狮迅l迵全瘸私邼
	}

}
//==========================================================
//	诏私幕詥煤	ESP8266_GetIPD
//
//	诏私佴艤煤	鼙取平台椎軜謩私邼
//
//	色酄氂幩矫?謭战謩时荬(詪訑10ms)
//
//	椎軜訋私煤	平台椎軜謩原始私邼
//
//	说姆煤		一同胃莽狮迅椎軜謩俦式一同矛穴要去址藬
//				骚ESP8266謩椎軜俦式为	"+IPD,x:yyy"	x蘸噎私邼婴讏矛yyy藝私邼艢蓾
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == rev_ok)								//骚诨迵藭为詨
		{
			ptrIPD = strstr((char *)rec_buf, "IPD,");				//虘谭pIPDq头
			if(ptrIPD == NULL)											//骚诨没謷战矛酄壟溗嘔PD头謩覔詸矛酃藝穴要謭战一堋矛斋一堋蝇诮狮吱謩时荬
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//謷战':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		vTaskDelay(5);													//覔时謭战
	} while(timeOut--);
	
	return NULL;														//蝇时酃未謷战矛椎軜酄曋钢?

}

void esp8266_init(void)
{
	esp8266_clear();
	
	Serial_Printf("%s","1,AT");
	while(ESP8266_SendCmd("AT\r\n", "OK"))//作用：检测模块是否在线，是否有响应。
		vTaskDelay(100);
	
	Serial_Printf("%s","2,AT+CWMODE=1");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))//设置模块工作模式为：Station 模式（连接路由器）
		vTaskDelay(100);
	
	Serial_Printf("%s","3,AT+CWDHCP=1,1");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))//开启 DHCP 功能.第一个 1：设置模式（Station）第二个 1：开启 DHCP，使其自动从路由器获取 IP 地址
		vTaskDelay(100);

	Serial_Printf("%s","4,cwjap");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))//作用：连接到指定 WiFi 热点
		vTaskDelay(100);

	Serial_Printf("%s","connect success");
	
}

void USART2_IRQHandler(void)
{
	
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET )
	{   
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		if(cnt >= sizeof(rec_buf))	cnt = 0; //防止串口被刷爆
		rec_buf[cnt++]= USART2->DR;		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
	}
	
}
//void USART2_IRQHandler(void)
//{

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //迵藭讗讖
//	{
//		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //貈止援酄氀凰⒀?
//		esp8266_buf[esp8266_cnt++] = USART2->DR;
//		
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
//	}

//}
