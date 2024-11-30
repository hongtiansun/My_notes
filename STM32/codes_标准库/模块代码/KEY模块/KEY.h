#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "Delay.h"

// 接口定义 两个按键 KEYPin1 返回1 KEYPin2 返回2
#define KEYGPIO_RCC RCC_APB2Periph_GPIOB
#define KEYPin1 GPIO_Pin_1
#define KEYPin2 GPIO_Pin_11
#define KEYGPIO GPIOB // 两个按键要在同一个组别
// 函数
void Key_Init();
uint8_t Key_GetNum();
#endif