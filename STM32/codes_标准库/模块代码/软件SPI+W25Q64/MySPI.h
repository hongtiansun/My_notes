#ifndef __MYSPI_H
#define __MYSPI_H

#include "stm32f10x.h"                  // Device header
//接口
#define SPIGPIO GPIOA
#define RCCSPI RCC_APB2Periph_GPIOA
#define SS GPIO_Pin_4
#define SCK GPIO_Pin_5
#define MOSI GPIO_Pin_7
#define MISO GPIO_Pin_6

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
