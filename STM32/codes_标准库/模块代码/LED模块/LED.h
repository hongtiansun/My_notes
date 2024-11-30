#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"

// LED1为GPIOA组的pin1
// LED2为GPIOA组的pin2

// 接口定义
#define LEDGPIO_RCC RCC_APB2Periph_GPIOA
#define LED1 GPIO_Pin_1
#define LED2 GPIO_Pin_2
#define LED_GPIO GPIOA

// 函数定义
void LED_Init(void);
void LED1_ON();
void LED1_OFF();
void LED2_ON();
void LED2_OFF();
void LED1_Turn();
void LED2_Turn();
#endif