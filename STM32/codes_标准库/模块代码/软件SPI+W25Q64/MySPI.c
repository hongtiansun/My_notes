#include "MySPI.h"

/*
@brief：写SS电平
@param: 要写的点位
@retvol: 无
*/
void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(SPIGPIO, SS, (BitAction)BitValue);
}
/*
@brief：写SCK电平
@param: 要写的点位
@retvol: 无
*/
void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(SPIGPIO, SCK, (BitAction)BitValue);
}
/*
@brief：写MOSI电平
@param: 要写的点位
@retvol: 无
*/
void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(SPIGPIO, MOSI, (BitAction)BitValue);
}
/*
@brief：读MISO电平
@param: 无
@retvol: 读到的电位
*/
uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(SPIGPIO, MISO);
}

//SPI初始化
void MySPI_Init(void)
{
	//GPIO时钟初始化
	RCC_APB2PeriphClockCmd(RCCSPI, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Pin = SS | SCK | MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIGPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIGPIO, &GPIO_InitStructure);
	
	MySPI_W_SS(1); //默认SS未使能
	MySPI_W_SCK(0); //SCK默认为0
}
//发送起始信号
void MySPI_Start(void)
{
	MySPI_W_SS(0); //SS下拉到0 从机SPI使能
}
//发送停止信号
void MySPI_Stop(void)
{
	MySPI_W_SS(1); //SS上拉到1 从机SPI失能
}

//W25Q64支持模式0和模式3
/*
@brief:交换字节，采用的是模式0，SCK默认低电平，第一个边沿采样输入(上升沿)，第二个边沿移除(下降沿)（先时钟沿再发生动作）
@param:写入字节
@retvol:读取字节
*/
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00; //待接受的字节
	
	for (i = 0; i < 8; i ++) //循环八次
	{
		//由于模式0，SS给下降沿时，从机W25Q64已经将数据发送到总线了，直接读取就可，而主机则需要先发送数据
		MySPI_W_MOSI(ByteSend & (0x80 >> i)); //主机向MOSI写入最高位
		MySPI_W_SCK(1); //SCK上升沿 采样输入，主机得到数据，从机也得到数据
		if (MySPI_R_MISO() == 1){ByteReceive |= (0x80 >> i);} //主机接收字节最高位
		MySPI_W_SCK(0); //SCK下降沿 从机再次移入下一位，进入再一个循环，主机也开始发送数据到总线
	}
	
	return ByteReceive; //返回
}
