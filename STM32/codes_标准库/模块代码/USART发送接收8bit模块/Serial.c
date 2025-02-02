#include "Serial.h"

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	//时钟配置
	RCC_APB2PeriphClockCmd(RCCUSART, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	//GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = TXPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = RXPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	
	//USART配置
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USARTMode;
	USART_InitStructure.USART_Parity = USARTParity;
	USART_InitStructure.USART_StopBits = USARTStopBits;
	USART_InitStructure.USART_WordLength = USARTWordLength;
	USART_Init(USART, &USART_InitStructure);
	//USART中断配置
	USART_ITConfig(USART, USARTIT, ENABLE);
	//NVIC配置
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

uint8_t Serial_GetRxFlag(void) //软件获取中断标志位 获取后清零
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void) //获取接收值
{
	return Serial_RxData;
}

void USARTIRQHandler(void)
{
	if (USART_GetITStatus(USART, USART_IT_RXNE) == SET)//获取到接收标志位
	{
		Serial_RxData = USART_ReceiveData(USART);//获取接收值
		Serial_RxFlag = 1;//置软件标志位
		USART_ClearITPendingBit(USART, USART_IT_RXNE);//清除RXNE标志位
	}
}

// 发送标志位可以自动清除，添加手动清楚也无所谓
// 循环GetFlag 当Flag为1时表示有数据到达可以接受
// 此方法每次只能接受8位数据
// 如果发送多个位数 只能接受最后一位。