#include "MyI2C.h"
/*
除了stop位，每个函数最后保持SCL为低电平，表示为SDA可以改变
SCL为高电平，表示SDA不能改变，数据发送阶段
*/
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOIIC, SCL, (BitAction)BitValue); //写SCL
	Delay_us(10); //延时
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOIIC, SDA, (BitAction)BitValue); //写SDA
	Delay_us(10); //延时
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOIIC, SDA); //读SDA
	Delay_us(10);
	return BitValue;
}
/*
@brief: I2C初始化
@param: void
@retval: void
*/
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCCIIC, ENABLE); //使能时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出 兼具输入功能
	GPIO_InitStructure.GPIO_Pin = SDA | SCL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOIIC, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOIIC, SDA | SCL); //全部拉高
}
/*
@brief: I2C开始信号
@param: void
@retval: void
*/

void MyI2C_Start(void)
{
	MyI2C_W_SDA(1); //SDA释放
	MyI2C_W_SCL(1); //SCL高电平
	MyI2C_W_SDA(0); //SDA 拉低
	MyI2C_W_SCL(0); //SCL拉低  时钟为拉低保持 方便修改SDA数据
}
/*
@brief: I2C结束信号
@param: void
@retval: void
*/
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0); //主机SDA拉低
	MyI2C_W_SCL(1); //SCL拉高
	MyI2C_W_SDA(1); //SDA释放 结束信号 时钟拉高保持
}
/*
@brief: I2C发送一个字节
@param: Byte: 要发送的字节
@retval: void
*/
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i)); //高位先发 设置SDA
		MyI2C_W_SCL(1);  //时钟拉高 发送数据
		MyI2C_W_SCL(0);  //时钟拉低 保持
	}
}
/*
@brief: I2C接收一个字节
@param: void
@retval: uint8_t: 接收到的字节
*/
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1); //释放SDA 从机控制SDA线 10ms延迟期间从机设置SDA线
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1); //拉高时钟 开始读取数据
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);} //读取数据 高位先发
		MyI2C_W_SCL(0); //释放时钟，从机继续设置SDA线
	}
	return Byte;
}
/*
@brief：I2C发送应答
@param: AckBit: 应答位
@retval: void
*/
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit); //设置SDA
	MyI2C_W_SCL(1); //拉高时钟 发送应答
	MyI2C_W_SCL(0); //释放时钟
}
/*
@brief: I2C接收应答
@param: void
@retval: uint8_t: 应答位
*/
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1); //释放SDA 从机控制SDA线
	MyI2C_W_SCL(1); //拉高时钟 开始读取数据
	AckBit = MyI2C_R_SDA(); //读取数据
	MyI2C_W_SCL(0); //释放时钟
	return AckBit;
}
