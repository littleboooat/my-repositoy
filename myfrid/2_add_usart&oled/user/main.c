
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "OLED.h"


TaskHandle_t mytaskhandle;
SemaphoreHandle_t UsartMutex;
SemaphoreHandle_t I2CMutex;

void mytask(void *para)
{
	while(1)
	{   GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
		GPIO_SetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
		Serial_Printf("i love you\r\n");
		OLED_ShowString(1,1,"          ");
		vTaskDelay(100);
		OLED_ShowString(1,1,"i love you");
		//OLED_ShowCenteredString(3,"happy" );
		//OLED_ShowChineseString(1, 1, "你好");
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
	
	UsartMutex=xSemaphoreCreateMutex();
	I2CMutex=xSemaphoreCreateMutex();
	OLED_Init();
	xTaskCreate(mytask,"mytask",128,NULL,2,&mytaskhandle);
	vTaskStartScheduler();
	while(1)
	{
								
	
	}
    return 0;
}
