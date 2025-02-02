#include "IC.h"

void IC_Init(void)
{
	//使能TIM3和GPIOA时钟
	RCC_APB1PeriphClockCmd(TIMRCC, ENABLE);
	RCC_APB2PeriphClockCmd(GPIORCC, ENABLE);

	//初始化GPIO接口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = TIMPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIMGPIO, &GPIO_InitStructure);

	//选择内部时钟
	TIM_InternalClockConfig(ICTIM);
	//时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = ICTIMCKD;//采样时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = ICTIMMode;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = ICTIMARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = ICTIMPSC;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //重复计数器高级定时器才有
	TIM_TimeBaseInit(ICTIM, &TIM_TimeBaseInitStructure);
	//输入捕获初始化
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = ICChannel; //通道1 选择输入通道1 每个定时器都有四个通道
	TIM_ICInitStructure.TIM_ICFilter = ICFilter; //输入滤波器值消除噪声 0·15 值越大滤波效果越好但是延时越大
	TIM_ICInitStructure.TIM_ICPolarity = ICPolarity; //上升沿捕获 可选参数 TIM_ICPolarity_Falling 下降沿捕获 TIM_ICPolarity_BothEdge 上升下降沿捕获
	TIM_ICInitStructure.TIM_ICPrescaler = ICPrescaler;//输入信号的分频系数 可选参数 TIM_ICPSC_DIV1 TIM_ICPSC_DIV2 TIM_ICPSC_DIV4
	TIM_ICInitStructure.TIM_ICSelection = ICSelection; //直接输入模式 可选参数 TIM_ICSelection_IndirectTI 简介通道(另一个通道) TIM_ICSelection_TRC 触发输入捕获信号
	TIM_ICInit(ICTIM, &TIM_ICInitStructure);
	//配置主从模式
	TIM_SelectInputTrigger(ICTIM, ICTIMTRG);//选择输入触发源 触发源为TI1FP1可选参数 TIM_TS_ITR0 TIM_TS_ITR1 TIM_TS_ITR2 TIM_TS_ITR3 TIM_TS_TI1FP1 TIM_TS_TI2FP2 TIM_TS_ETRF
	TIM_SelectSlaveMode(ICTIM, ICTIMSM);//选择从模式 从模式为复位可选参数 TIM_SlaveMode_Trigger TIM_SlaveMode_Reset TIM_SlaveMode_Gated TIM_SlaveMode_Trigger
	
	TIM_Cmd(ICTIM, ENABLE);
}

//原理详解
//输入信号上升沿到来时TIM3的CNT寄存器的值被捕获到CCR1寄存器中 同时触发CNT寄存器清零
//此时CCR1寄存器的值就是输入信号的周期
//标准频率为72Mhz/72=1Mhz
uint32_t IC_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(ICTIM) + 1); //返回频率
}
