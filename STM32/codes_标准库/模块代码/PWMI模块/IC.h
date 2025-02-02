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
uint32_t IC_GetDuty(void);

#endif
