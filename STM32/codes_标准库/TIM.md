
## 定时器中断实现

头文件接口
```C
#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
//定时器接口 //CNT = 72MHZ / (PSC +1) / (ARR + 1)
//TIM接口
#define TIMRCC RCC_APB1Periph_TIM2
#define TIM TIM2
#define TIMClockDivision TIM_CKD_DIV1 //可选参数 TIM_CKD_DIV2 TIM_CKD_DIV4
#define TIMCounterMode TIM_CounterMode_Up //可选参数 TIM_CounterMode_Down
#define TIMARR 10000-1
#define TIMPSC 7200-1
//NVIC接口
#define NVICGroup NVIC_PriorityGroup_2 //优先级组别
#define NVICIRQChannel TIM2_IRQn //中断号
#define NVICChannelPreemptionPriority 2 //抢占优先级
#define NVICChannelSubPriority 1 //响应优先级
//中断处理函数
#define TIM_IRQHandler TIM2_IRQHandler

//声明函数 变量
extern int16_t NUM;
void Timer_Init();

#endif
```

定时器初始化
```C
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
		NUM++;
	}
	TIM_ClearITPendingBit(TIM,TIM_IT_Update);
}
```

## 输出比较
```C
#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h"

//接口

//PWM接口设置
#define PWMRCC RCC_APB1Periph_TIM2
#define PWMTIM TIM2
#define PWMGPIO GPIOA
#define PWMGPIORCC RCC_APB2Periph_GPIOA
#define PWMpin GPIO_Pin_1|GPIO_Pin_0
//TIM设置
#define TIMClockDivision TIM_CKD_DIV1
#define TIMCounterMode TIM_CounterMode_Up
#define TIMARR 100-1
#define TIMPsc 720-1
//OC设置
#define TIMOCMode TIM_OCMode_PWM2
#define TIMOCPolarity TIM_OCPolarity_High
#define TIMOCCRR 80
#define TIMOCInit TIM_OC2Init
void PWM_Init();
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
#endif
```
配置
```C
#include "PWM.h"

void PWM_Init(){
	//开启时钟 TIM2 挂载在APB1总线上
	RCC_APB1PeriphClockCmd(PWMRCC,ENABLE);
	//选择时基时钟 内部时钟 (时钟源)
	TIM_InternalClockConfig(PWMTIM);

	//配置GPIOA引脚 PWM输出
	RCC_APB2PeriphClockCmd(PWMGPIORCC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=PWMpin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(PWMGPIO,&GPIO_InitStructure);

	//配置引脚重映射
	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//复用到A15
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭原ATAG调试功能*/
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIMClockDivision;//采样频率分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIMCounterMode;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=TIMARR;//ARR自动重装寄存器
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIMPsc;//PSC预分频器值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器的值 高级定时器才有
	TIM_TimeBaseInit(PWMTIM,&TIM_TimeBaseInitStructure);
	TIM_ClearFlag(PWMTIM,TIM_FLAG_Update);//初始化完对中断标志位清零，防止复位时立即进入中断

	//配置输出比较单元
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);///变量太多 赋初值 
	TIM_OCInitStructure.TIM_OCMode=TIMOCMode; //设置模式
	TIM_OCInitStructure.TIM_OCPolarity=TIMOCPolarity;  //设置极性
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse=TIMOCCRR;  // 设置CRR
	TIMOCInit(PWMTIM,&TIM_OCInitStructure);
	//启动定时器
	TIM_Cmd(PWMTIM,ENABLE);
}

void PWM_SetCompare1(uint16_t Compare){ //设置通道1比较值
	TIM_SetCompare1(PWMTIM,Compare);
}
void PWM_SetCompare2(uint16_t Compare){//设置通道2比较值
	TIM_SetCompare2(PWMTIM,Compare);
}
```

## 输入捕获

IC接口定义
```C
#ifndef __IC_H
#define __IC_H
#include "stm32f10x.h"                  // Device header
//接口定义
//时钟
#define TIMRCC RCC_APB1Periph_TIM3
#define GPIORCC RCC_APB2Periph_GPIOA
//GPIO
#define TIMGPIO GPIOA 
#define TIMPin GPIO_Pin_6
//定时器
#define ICTIM TIM3
#define ICTIMCKD TIM_CKD_DIV1
#define ICTIMMode TIM_CounterMode_Up
#define ICTIMPSC 72 - 1
#define ICTIMARR 65536 - 1
//IC模块
#define ICChannel TIM_Channel_1 //通道1 选择输入通道1 每个定时器都有四个通道
#define ICFilter 0xF //输入滤波器值消除噪声 0·15 值越大滤波效果越好但是延时越大
#define ICPolarity TIM_ICPolarity_Rising //上升沿捕获 可选参数 TIM_ICPolarity_Falling 下降沿捕获 TIM_ICPolarity_BothEdge 上升下降沿捕获
#define ICSelection TIM_ICSelection_DirectTI //直接输入模式 可选参数 TIM_ICSelection_IndirectTI 简介通道(另一个通道) TIM_ICSelection_TRC 触发输入捕获信号
#define ICPrescaler TIM_ICPSC_DIV1 //输入信号的分频系数 可选参数 TIM_ICPSC_DIV1 TIM_ICPSC_DIV2 TIM_ICPSC_DIV4
//主从模式配置
#define ICTIMTRG TIM_TS_TI1FP1 //从模式输入触发源
#define ICTIMSM TIM_SlaveMode_Reset //从模式响应复位

void IC_Init(void);
uint32_t IC_GetFreq(void);

#endif
```
IC定义
```C
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
```

## PWMI模式

区别只在于IC初始化用PWMI配置
配置
```C
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
```

## 旋转编码器接口

```C
void Encoder_Init(void)
{
	//使能时钟
	RCC_APB1PeriphClockCmd(RCCTIM, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = EnGPIOA | EnGPIOB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EnGPIO, &GPIO_InitStructure);
	//TIM初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不起作用
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//不起作用
	TIM_TimeBaseInitStructure.TIM_Period = TIMARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = TIMPSC;		//不起作用Encode接管
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM, &TIM_TimeBaseInitStructure);
	//IC初始化
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = ICChannelA;
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;
	TIM_ICInit(TIM, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = ICChannelB;
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;
	TIM_ICInit(TIM, &TIM_ICInitStructure);
	//编码器初始化
	TIM_EncoderInterfaceConfig(TIM, EncoderMode, EncoderPolarityA, EncoderPolarityB);
	/**
	 * @brief  Configures the TIMx Encoder Interface.
	 * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
	 * @param  TIM_EncoderMode: specifies the TIMx Encoder Mode.
	 *   This parameter can be one of the following values:
	 *     @arg TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
	 *     @arg TIM_EncoderMode_TI2: Counter counts on TI2FP2 edge depending on TI1FP1 level.
	 *     @arg TIM_EncoderMode_TI12: Counter counts on both TI1FP1 and TI2FP2 edges depending
	 *                                on the level of the other input.
	 * @param  TIM_IC1Polarity: specifies the IC1 Polarity
	 *   This parameter can be one of the following values:
	 *     @arg TIM_ICPolarity_Falling: IC Falling edge.
	 *     @arg TIM_ICPolarity_Rising: IC Rising edge.
	 * @param  TIM_IC2Polarity: specifies the IC2 Polarity
	 *   This parameter can be one of the following values:
	 *     @arg TIM_ICPolarity_Falling: IC Falling edge.
	 *     @arg TIM_ICPolarity_Rising: IC Rising edge.
	 * @retval None
	 */
	TIM_Cmd(TIM, ENABLE);
}

int16_t Encoder_Get(void)
{
	/*
	int16_t Temp;
	Temp = TIM_GetCounter(TIM);//读取计数器CNT
	TIM_SetCounter(TIM, 0); //清零计数器CNT
	return Temp;*/
	return TIM_GetCounter(TIM)/4;
}
```