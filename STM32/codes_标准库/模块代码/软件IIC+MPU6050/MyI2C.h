#ifndef __MYI2C_H
#define __MYI2C_H
#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define RCCIIC RCC_APB2Periph_GPIOB //时钟
#define GPIOIIC GPIOB //端口
#define SDA GPIO_Pin_11 //SDA引脚
#define SCL GPIO_Pin_10 //SCL引脚

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

#endif
