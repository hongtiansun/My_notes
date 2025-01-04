#ifndef __ENCODER_H
#define __ENCODER_H
#include "stm32f10x.h"
//接口定义
//GPIO接口
#define ENCODERCC RCC_APB2Periph_GPIOB //定义编码器引脚时钟
#define ENCODEGPIO GPIOB //定义编码器引脚
#define ENCODEPinA GPIO_Pin_0 //定义编码器A引脚
#define ENCODEPinB GPIO_Pin_1 //定义编码器B引脚
//AFIO接口定义
#define AFIOPortSourceGPIO GPIO_PortSourceGPIOB //定义编码器引脚
#define AFIOPinASourceGPIO GPIO_PinSource0 //定义编码器A引脚
#define AFIOPinBSourceGPIO GPIO_PinSource1 //定义编码器B引脚
//EXTI接口定义
#define EXTILineA EXTI_Line0 //定义编码器A引脚中断线
#define EXTILineB EXTI_Line1 //定义编码器B引脚中断线
#define EXTIMode EXTI_Mode_Interrupt //定义中断模式 可选参数为EXTI_Mode_Event 
#define EXTITrigger EXTI_Trigger_Falling //定义下降沿触发 可选参数为EXTI_Trigger_Rising
//NVIC接口定义
#define NVICGroup NVIC_PriorityGroup_2 //定义优先级组
#define NVICIRQChannelA EXTI0_IRQn //定义编码器A引脚中断通道
#define NVICIRQChannelB EXTI1_IRQn //定义编码器B引脚中断通道
#define NVICPreemptionPriority 1 //定义抢占优先级
#define NVICSubPriority 1 //定义响应优先级
//IRQ函数
#define EXTIAIRQ EXTI0_IRQHandler //定义编码器A引脚中断函数
#define EXTIBIRQ EXTI1_IRQHandler //定义编码器B引脚中断函数

//函数声明
void Encoder_Init();
int16_t Get_Encoder_Count();
#endif