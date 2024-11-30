#include "BUZZER.h"

void Buzzer_Init(void)
{

	RCC_APB2PeriphClockCmd(BeepGPIO_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BeepPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BeepGPIO, &GPIO_InitStructure);

	GPIO_SetBits(BeepGPIO, BeepPin);
}

void Buzzer_ON()
{
	GPIO_ResetBits(BeepGPIO, BeepPin);
}
void Buzzer_OFF()
{
	GPIO_SetBits(BeepGPIO, BeepPin);
}