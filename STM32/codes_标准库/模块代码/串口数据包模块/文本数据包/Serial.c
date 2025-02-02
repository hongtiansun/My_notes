#include "Serial.h"

char Serial_RxPacket[100];				//"@MSG\r\n" \r\n为回车转义字符 指令示例：@LED_ON[回车]
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	//时钟配置
	RCC_APB2PeriphClockCmd(RCCUSART, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = TXPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = RXPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	//USART
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USARTMode;
	USART_InitStructure.USART_Parity = USARTParity;
	USART_InitStructure.USART_StopBits = USARTStopBits;
	USART_InitStructure.USART_WordLength = USARTWordLength;
	USART_Init(USART, &USART_InitStructure);
	
	USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVICGroup);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = NVICChannel;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVICPrePriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVICSubPriority;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART, Byte);
	while (USART_GetFlagStatus(USART, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

void USARTIRQHandler(void) //接收包中断
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART);
		
		if (RxState == 0) //状态0
		{
			if (RxData == '@' && Serial_RxFlag == 0)//添加标志位为0 保证上一次指令执行完才能接受下一次指令
			{
				RxState = 1;//状态2
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '\r')//结束位
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
		else if (RxState == 2)//状态2
		{
			if (RxData == '\n')
			{
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\0';//字符串赋值\0
				Serial_RxFlag = 1;//接收标志位1
			}
		}
		
		USART_ClearITPendingBit(USART, USART_IT_RXNE);
	}
}
