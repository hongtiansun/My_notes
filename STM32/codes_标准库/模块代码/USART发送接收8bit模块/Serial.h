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
/*标志位补充
  *     @arg USART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5)
  *     @arg USART_FLAG_LBD:  LIN Break detection flag
  *     @arg USART_FLAG_TXE:  Transmit data register empty flag
  *     @arg USART_FLAG_TC:   Transmission Complete flag
  *     @arg USART_FLAG_RXNE: Receive data register not empty flag
  *     @arg USART_FLAG_IDLE: Idle Line detection flag
  *     @arg USART_FLAG_ORE:  OverRun Error flag
  *     @arg USART_FLAG_NE:   Noise Error flag
  *     @arg USART_FLAG_FE:   Framing Error flag
  *     @arg USART_FLAG_PE:   Parity Error flag

  * 	USART_FLAG_TXE：发送数据寄存器空标志位。当该标志位置位时，表示可以向发送数据寄存器写入新的数据。
		USART_FLAG_TC：传输完成标志位。当该标志位置位时，表示所有数据已经发送完成。
		USART_FLAG_RXNE：接收数据寄存器非空标志位。当该标志位置位时，表示接收到的数据可以读取。
		USART_FLAG_IDLE：空闲线路检测标志位。当该标志位置位时，表示线路处于空闲状态。
		USART_FLAG_ORE：溢出错误标志位。当该标志位置位时，表示接收数据寄存器溢出。
		USART_FLAG_NE：噪声错误标志位。当该标志位置位时，表示接收到的数据存在噪声错误。
		USART_FLAG_FE：帧错误标志位。当该标志位置位时，表示接收到的数据帧存在帧错误。
		USART_FLAG_PE：奇偶校验错误标志位。当该标志位置位时，表示接收到的数据存在奇偶校验错误
  * */

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
