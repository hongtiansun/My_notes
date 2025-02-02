#include "Timer.h"
int16_t NUM;

void Timer_Init(){
	//开启时钟 TIM2 挂载在APB1总线上
	RCC_APB1PeriphClockCmd(TIMRCC,ENABLE);

	//选择时基时钟 内部时钟
	TIM_InternalClockConfig(TIM);

	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIMClockDivision;//采样频率分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIMCounterMode;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=TIMARR;//ARR自动重装寄存器
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIMPSC;//PSC预分频器值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器的值 高级定时器才有
	TIM_TimeBaseInit(TIM,&TIM_TimeBaseInitStructure);
	TIM_ClearFlag(TIM,TIM_FLAG_Update);//初始化完对中断标志位清零，防止复位时立即进入中断

	//配置中断输出
	TIM_ITConfig(TIM,TIM_IT_Update,ENABLE);

	//配置NVIC
	NVIC_PriorityGroupConfig(NVICGroup);
	NVIC_InitTypeDef NVIC_InitTypeDefStructure;
	NVIC_InitTypeDefStructure.NVIC_IRQChannel=NVICIRQChannel;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelPreemptionPriority=NVICChannelPreemptionPriority;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelSubPriority=NVICChannelSubPriority;
	NVIC_Init(&NVIC_InitTypeDefStructure);
	//启动定时器
	TIM_Cmd(TIM,ENABLE);
}

void TIM_IRQHandler(void){
	if(TIM_GetITStatus(TIM,TIM_IT_Update)==SET){
		NUM = Encoder_Get();
	}
	TIM_ClearITPendingBit(TIM,TIM_IT_Update);
}