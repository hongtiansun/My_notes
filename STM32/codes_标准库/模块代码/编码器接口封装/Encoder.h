#ifndef __ENCODER_H
#define __ENCODER_H
#include "stm32f10x.h"                  // Device header

//RCC接口
#define RCCTIM RCC_APB1Periph_TIM3
#define RCCGPIO RCC_APB2Periph_GPIOA
//GPIO接口
#define EnGPIO GPIOA
#define EnGPIOA GPIO_Pin_6
#define EnGPIOB GPIO_Pin_7
//TIM接口
#define TIM TIM3
#define TIMARR 65536-1
#define TIMPSC 1-1
//IC接口
#define ICChannelA TIM_Channel_1
#define ICFilter 0xF
#define ICChannelB TIM_Channel_2
//编码器接口
#define EncoderMode TIM_EncoderMode_TI12
#define EncoderPolarityA TIM_ICPolarity_Falling
#define EncoderPolarityB TIM_ICPolarity_Rising

void Encoder_Init(void);
int16_t Encoder_Get(void);

#endif
