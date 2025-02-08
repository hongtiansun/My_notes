#include "MySPI.h"

//SPI设备使能
void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(SPIGPIO, SS, (BitAction)BitValue);
}

//SPI初始化
void MySPI_Init(void)
{
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCCGPIOSPI, ENABLE);
	RCC_APB2PeriphClockCmd(RCCSPI, ENABLE);
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin = SS;  //使能引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIGPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽
	GPIO_InitStructure.GPIO_Pin = SCK | MOSI; //SCK MOSI由SPI外设控制
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIGPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = MISO; //MISO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIGPIO, &GPIO_InitStructure);
	//SPI初始化
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPIMode; //SPI主模式
	SPI_InitStructure.SPI_Direction = SPIDirection; // 双选全双工 标准模式
	SPI_InitStructure.SPI_DataSize = SPIData; //八位数据帧
	SPI_InitStructure.SPI_FirstBit = SPIFBit; //高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPIPsc; //波特率128分频
	SPI_InitStructure.SPI_CPOL = SPICPOL; // 配置模式 CLK默认低
	SPI_InitStructure.SPI_CPHA = SPICPHA; // 配置模式 第一个边沿采样 模式0
	SPI_InitStructure.SPI_NSS = SPINSS; //软件控制NSS
	SPI_InitStructure.SPI_CRCPolynomial = SPINSS; //CRC校验位
	SPI_Init(SPI, &SPI_InitStructure);
	
	SPI_Cmd(SPI, ENABLE);
	
	MySPI_W_SS(1); //SPI拉高
}

void MySPI_Start(void)
{
	MySPI_W_SS(0); //开始SS使能时钟
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1); //SPI失能
}

/*交换字节*/
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) != SET); //等待TXE标志位为1
	
	SPI_I2S_SendData(SPI, ByteSend); //写入Byte到SPI DR
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET); //等待RXNE标志位为1 标志着发送接收都完成了(发送和接收是一体的)
	
	return SPI_I2S_ReceiveData(SPI); //读取Byte返回 //TXE RXNE都可以有硬件自动清除
}
