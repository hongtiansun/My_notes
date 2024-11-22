#ifndef __LED_H
#define __LED_H
//LED1为GPIOA组的pin1
//LED2为GPIOA组的pin2

void LED_Init(void);
void LED1_ON();
void LED1_OFF();
void LED2_ON();
void LED2_OFF();
void LED1_Turn();
void LED2_Turn();
#endif