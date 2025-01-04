#ifndef __BUZZ_H
#define __BUZZ_H
#include "stm32f10x.h"

//接口定义
#define BeepGPIO_RCC RCC_APB2Periph_GPIOB //Beep GPIO 所在外设组别时钟
#define BeepGPIO GPIOB //Beep所在组别
#define BeepPin GPIO_Pin_12 //Beep的引脚

//函数定义
void Buzzer_Init(void);
void Buzzer_ON();
void Buzzer_OFF();
#endif