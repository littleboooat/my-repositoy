#include "stm32f10x.h"
#include "time.h"
#include"usart.h"
uint16_t mytime[]={2025,7,4,15,28,0};
void myRTCsettime(void);

void myRTC_Init(void)	
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);  
	PWR_BackupAccessCmd(ENABLE);
	
//	Serial_Printf("%x",BKP_ReadBackupRegister(BKP_DR1));
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A5)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();        
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();
		
		myRTCsettime();
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
		
	}
	else
	{		
        RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}		
}

void myRTCsettime(void)
{
	time_t myUnix;
	struct tm time_data;
	time_data.tm_year=mytime[0]-1900;	
Serial_Printf("%d",time_data.tm_year);
	time_data.tm_mon=mytime[1]-1;
	time_data.tm_mday=mytime[2];
	time_data.tm_hour=mytime[3];
	time_data.tm_min=mytime[4];
	time_data.tm_sec=mytime[5];
	myUnix=mktime(&time_data)-8*60*60;

	RTC_SetCounter(myUnix);
//Serial_Printf("%d",myUnix);
	RTC_WaitForLastTask();
		
}

void myRTCreadtime(void)
{
	
	time_t myUnix;
	struct tm time_data;	
	myUnix=RTC_GetCounter()+8*60*60;	
	time_data=*localtime(&myUnix);
	mytime[0]=time_data.tm_year+1900;
	mytime[1]=time_data.tm_mon+1;
	mytime[2]=time_data.tm_mday;
	mytime[3]=time_data.tm_hour;
	mytime[4]=time_data.tm_min;
	mytime[5]=time_data.tm_sec;
	
}