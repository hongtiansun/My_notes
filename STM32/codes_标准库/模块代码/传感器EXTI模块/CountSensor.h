#ifndef __COUNTSENSOR_H
#define __COUNTSENSOR_H

#include "stm32f10x.h"
//接口定义
//配置GPIO引脚接口
#define EXTIPinRCC RCC_APB2Periph_GPIOB //中断引脚时钟
#define EXTIPin GPIO_Pin_13 //中断引脚
#define EXTIPort GPIOB //中断引脚组别
//配置AFIO引脚接口
#define AFIOPortSourceGPIO GPIO_PortSourceGPIOB //AFIO引脚组别
#define AFIOExtiLineSource GPIO_PinSource13 //AFIO中断引脚
//配置EXTI引脚接口
#define EXTILine EXTI_Line13 //中断线 可选参数 EXTI_Line0 EXTI_Line1 ... EXTI_Line15
#define EXTIMode EXTI_Mode_Interrupt //中断模式 可选参数 EXTI_Mode_Event
#define EXTITRIG EXTI_Trigger_Falling //中断触发方式 可选参数 EXTI_Trigger_Rising EXTI_Trigger_Rising_Falling
//配置NVIC引脚接口
#define NVICPeriphGroup NVIC_PriorityGroup_2 //中断优先级组别
#define NVICPeriphChannel EXTI15_10_IRQn //中断通道 可选参数 EXTI0_IRQn EXTI1_IRQn ... EXTI15_10_IRQn
#define NVICPeriphPreemptionPriority 1 //抢占优先级
#define NVICPeriphSubPriority 1 //响应优先级
//配置中断处理函数
#define EXTIIRQHandler EXTI15_10_IRQHandler //中断处理函数

//全局变量
extern uint16_t CountSensor_Count;
//函数声明
void CountSensor_Init();
uint16_t Get_CountSensor_Count();
#endif