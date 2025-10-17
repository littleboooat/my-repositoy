#include "stm32f10x.h"
//==========================================================
//说明:用于提供vTaskGetRunTimeStats(runTimeStats)所需要的时间。
//开始调度之前会自动调用一次#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()   TIM2_Config()。用于初始化tim2
//运行时会不断调用#define portGET_RUN_TIME_COUNTER_VALUE()           Get_64us_Time()。获取当前系统时间
//==========================================================
volatile uint16_t high_word = 0; // 计数器溢出次数（高16位）

// TIM2 初始化配置，计数频率 1MHz (1us计数一次)
void TIM2_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // 72MHz / 72 = 1MHz（1μs）
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;     // 16位最大值
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);               // 清除溢出标志
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);          // 使能溢出中断

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10; // 优先级根据需要设置
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);  // 启动定时器
}

// TIM2 中断服务函数（溢出时调用）
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        high_word++;                   // 溢出计数加1
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志
    }
}

// 返回32位扩展计数值，单位1微秒

uint32_t Get_64us_Time(void)
{
    uint32_t cnt1, cnt2, high;
    do {
        high = high_word;
        cnt1 = TIM2->CNT;
        cnt2 = TIM2->CNT;
    } while (cnt2 < cnt1); // 防止计数器回绕导致读取错误

    return (high << 16) | cnt2;//一个32位的计数值，1表示1us。可以计数2的32次方。
}