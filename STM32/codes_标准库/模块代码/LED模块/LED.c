#include "LED.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(LEDGPIO_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED1 | LED2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(LED_GPIO, &GPIO_InitStructure);

	GPIO_SetBits(LED_GPIO, LED1 | LED2);
}

void LED1_ON()
{
	GPIO_ResetBits(LED_GPIO, LED1);
}
void LED1_OFF()
{
	GPIO_SetBits(LED_GPIO, LED1);
}
void LED2_ON()
{
	GPIO_ResetBits(LED_GPIO, LED2);
}
void LED2_OFF()
{
	GPIO_SetBits(LED_GPIO, LED2);
}

void LED1_Turn()
{
	if (GPIO_ReadOutputDataBit(LED_GPIO, LED1) == 0)
	{
		GPIO_SetBits(LED_GPIO, LED1);
	}
	else
	{
		GPIO_ResetBits(LED_GPIO, LED1);
	}
}
void LED2_Turn()
{ // 疑惑？？？
	if (GPIO_ReadOutputDataBit(LED_GPIO, LED2) == 0)
	{
		GPIO_SetBits(LED_GPIO, LED2);
	}
	else
	{
		GPIO_ResetBits(LED_GPIO, LED2);
	}
}