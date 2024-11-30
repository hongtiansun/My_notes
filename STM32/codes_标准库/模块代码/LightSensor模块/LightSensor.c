#include "LightSensor.h"

void LightSensor_Init()
{
	RCC_APB2PeriphClockCmd(LightSensor_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = LightSensor_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(LightSensor_GPIO, &GPIO_InitStructure);
}

uint8_t LightSenor_Get()
{
	return GPIO_ReadInputDataBit(LightSensor_GPIO, LightSensor_Pin);
}