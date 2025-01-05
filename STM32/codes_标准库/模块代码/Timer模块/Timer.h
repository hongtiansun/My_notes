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