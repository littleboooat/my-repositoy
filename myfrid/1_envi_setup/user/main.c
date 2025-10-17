#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                  // Device header
#include "Task.h" 

TaskHandle_t mytaskhandle;
void mytask(void *para)
{
	while(1)
	{   GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
		GPIO_SetBits(GPIOC, GPIO_Pin_13);	
		vTaskDelay (500);
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
	
	xTaskCreate(mytask,"mytask",128,NULL,2,&mytaskhandle);
	vTaskStartScheduler();
	while(1)
	{
								
	
	}
    return 0;
}
