#ifndef __LIGHT_H
#define __LIGHT_H
#include "stm32f10x.h"

// 接口定义
#define LightSensor_RCC RCC_APB2Periph_GPIOB
#define LightSensor_Pin GPIO_Pin_13
#define LightSensor_GPIO GPIOB
// 函数定义
void LightSensor_Init();
uint8_t LightSenor_Get();
#endif