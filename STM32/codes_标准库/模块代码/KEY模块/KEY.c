#include "KEY.h"

void Key_Init()
{
	RCC_APB2PeriphClockCmd(KEYGPIO_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEYPin1 | KEYPin2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(KEYGPIO, &GPIO_InitStructure);
}

uint8_t Key_GetNum()
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(KEYGPIO, KEYPin1) == 0)
	{
		Delay_ms(20); // 按下消抖
		while (GPIO_ReadInputDataBit(KEYGPIO, KEYPin1) == 0)
			;		  // 等待松手
		Delay_ms(20); // 松开消抖
		KeyNum = 1;	  // 表示按下
	}
	if (GPIO_ReadInputDataBit(KEYGPIO, KEYPin2) == 0)
	{
		Delay_ms(20); // 按下消抖
		while (GPIO_ReadInputDataBit(KEYGPIO, KEYPin2) == 0)
			;		  // 等待松手
		Delay_ms(20); // 松开消抖
		KeyNum = 2;	  // 表示按下
	}
	return KeyNum;
}
