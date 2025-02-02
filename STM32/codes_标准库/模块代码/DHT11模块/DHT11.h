#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"
#include "Delay.h"
//接口
#define RCCDHT11 RCC_APB2Periph_GPIOA
#define DHT11_IO GPIO_Pin_7
#define DHTGPIO GPIOA
#define DHTMode GPIO_Mode_Out_OD //开漏输出 可以实现输入输出的切换

void DHT11_Init(void);
void WriteIO(uint8_t BitValue);
void DHT11_Start();
uint8_t DHT11_Check();
uint8_t DHT11_ReadByte();
uint8_t DHT11_ReadData(uint8_t *Hum,uint8_t *FHum,uint8_t *Temp,uint8_t *FTemp);
#endif