#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#define RCCUSART RCC_APB2Periph_USART1
#define RCCGPIO RCC_APB2Periph_GPIOA

#define GPIO GPIOA
#define TXPin GPIO_Pin_9
#define RXPin GPIO_Pin_10

#define USART USART1
#define BaudRate 9600
#define USARTMode USART_Mode_Tx | USART_Mode_Rx//发送模式 可选参数有USART_Mode_Rx接收模式 USART_Mode_Tx发送模式
#define USARTParity USART_Parity_No//校验位 可选参数有USART_Parity_No无校验 USART_Parity_Even偶校验 USART_Parity_Odd奇校验
#define USARTStopBits USART_StopBits_1//停止位 可选参数有USART_StopBits_1一个停止位 USART_StopBits_0_5 0.5个停止位 USART_StopBits_2 两个停止位 USART_StopBits_1_5 1.5个停止位
#define USARTWordLength USART_WordLength_8b//数据位长度 可选参数有USART_WordLength_8b 8位数据 USART_WordLength_9b 9位数据
#define USARTIT USART_IT_RXNE

#define NVICGroup NVIC_PriorityGroup_2
#define NVICChannel USART1_IRQn
#define NVICPrePriority 1
#define NVICSubPriority 1

#define USARTIRQHandler USART1_IRQHandler

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_SendPacket(void);
uint8_t Serial_GetRxFlag(void);

#endif
