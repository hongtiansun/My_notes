#include "DS18B20.h"

/**
  * @brief  DS18B20开始温度变换
  * @param  无
  * @retval 无
  */
void DS18B20_ConvertT(void)
{
	OneWire_Init();//初始化OneWire总线
	OneWire_SendByte(DS18B20_SKIP_ROM);//跳过ROM
	OneWire_SendByte(DS18B20_CONVERT_T);//开始温度转换
}

/**
  * @brief  DS18B20读取温度
  * @param  无
  * @retval 温度数值
  */
float DS18B20_ReadT(void)
{
	unsigned char TLSB,TMSB;//温度值 低字节 高字节
	int Temp;
	float T;

	OneWire_Init();//初始化OneWire总线
	OneWire_SendByte(DS18B20_SKIP_ROM);//跳过ROM
	OneWire_SendByte(DS18B20_READ_SCRATCHPAD);//读取暂存器指令
	TLSB=OneWire_ReceiveByte();//读取温度值 低字节
	TMSB=OneWire_ReceiveByte();//读取温度值 高字节
    //计算温度
	Temp=(TMSB<<8)|TLSB; //合成两个字节的温度值 MSB LSB
	T=Temp/16.0;//右移4位，除以16，得到温度值
	return T;
}
