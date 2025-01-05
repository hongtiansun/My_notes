
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

