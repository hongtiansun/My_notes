#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f10x.h"                  // Device header
#include "PWM.h"

//接口
#define Servo_PWMCompare PWM_SetCompare1

void Servo_Init(void);
void Servo_SetAngle(float Angle);
#endif
