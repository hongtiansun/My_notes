#ifndef __ONWIRE_H
#define __ONWIRE_H
#include "stm32f10x.h" 
#include "delay.h"

//引脚定义
#define RCCOneWire RCC_APB2Periph_GPIOA
#define OneWire_GPIO GPIOA
#define OneWire_Pin GPIO_Pin_6

//高低电平
#define OneWire_DQ   GPIO_ReadInputDataBit(OneWire_GPIO,OneWire_Pin)
#define OneWire_Low  GPIO_ResetBits(OneWire_GPIO,OneWire_Pin)
#define OneWire_High GPIO_SetBits(OneWire_GPIO,OneWire_Pin)	

//GPIO模式
#define IN 0
#define OUT 1

uint8_t OneWire_Init(void);
void GPIOMode(uint8_t Mode);
void OneWire_SendBit(unsigned char Bit);
uint8_t OneWire_ReceiveBit(void);
void OneWire_SendByte(unsigned char Byte);
uint8_t OneWire_ReceiveByte(void);
#endif // __ONWIRE_H