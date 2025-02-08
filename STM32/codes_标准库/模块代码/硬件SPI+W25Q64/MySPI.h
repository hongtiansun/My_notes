#ifndef __MYSPI_H
#define __MYSPI_H

#include "stm32f10x.h"                  // Device header

//接口GPIO
#define SPIGPIO GPIOA
#define RCCGPIOSPI RCC_APB2Periph_GPIOA
#define SS GPIO_Pin_4
#define SCK GPIO_Pin_5
#define MOSI GPIO_Pin_7
#define MISO GPIO_Pin_6
//SPI初始化
#define RCCSPI RCC_APB2Periph_SPI1
#define SPI SPI1
#define SPIMode SPI_Mode_Master
#define SPIDirection SPI_Direction_2Lines_FullDuplex
#define SPIData SPI_DataSize_8b
#define SPIFBit SPI_FirstBit_MSB
#define SPIPsc SPI_BaudRatePrescaler_128
#define SPICPOL SPI_CPOL_Low
#define SPICPHA SPI_CPHA_1Edge
#define SPINSS SPI_NSS_Soft
#define SPICRC 7

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
