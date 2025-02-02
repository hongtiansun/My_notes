#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"                  // Device header
#include "PWM.h"

#define MOTORCC RCC_APB2Periph_GPIOA
#define MOTORGPIO GPIOA
#define MOTORPIN1 GPIO_Pin_4
#define MOTORPIN2 GPIO_Pin_5

void Motor_Init(void);
void Motor_SetSpeed(int8_t Speed);

#endif
