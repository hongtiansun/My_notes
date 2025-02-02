#include "IC.h"

void IC_Init(void)
{
	//时钟使能
	RCC_APB1PeriphClockCmd(TIMRCC, ENABLE);
	RCC_APB2PeriphClockCmd(GPIORCC, ENABLE);
	//GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = TIMPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIMGPIO, &GPIO_InitStructure);

	TIM_InternalClockConfig(ICTIM); //内部时钟
	//定时器配置	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = ICTIMCKD;
	TIM_TimeBaseInitStructure.TIM_CounterMode = ICTIMMode;
	TIM_TimeBaseInitStructure.TIM_Period = ICTIMARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = ICTIMPSC;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(ICTIM, &TIM_TimeBaseInitStructure);
	//输入捕获配置
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = ICChannel;
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;
	TIM_ICInitStructure.TIM_ICPolarity = ICPolarity;
	TIM_ICInitStructure.TIM_ICPrescaler = ICPrescaler;
	TIM_ICInitStructure.TIM_ICSelection = ICSelection;
	TIM_PWMIConfig(ICTIM, &TIM_ICInitStructure); //PWMI初始化 自动设置反向设置
	//主从模式配置
	TIM_SelectInputTrigger(ICTIM, ICTIMTRG); //选择输入触发源 直接触发捕获上升沿到CRR1 PWMI简介触发捕获下降沿到CRR2
	TIM_SelectSlaveMode(ICTIM, ICTIMSM);
	//定时器使能
	TIM_Cmd(ICTIM, ENABLE);
}

uint32_t IC_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(ICTIM) + 1);
}

uint32_t IC_GetDuty(void)
{
	return (TIM_GetCapture2(ICTIM) + 1) * 100 / (TIM_GetCapture1(ICTIM) + 1); //占空比 通道二计数值/通道一计数值
}
