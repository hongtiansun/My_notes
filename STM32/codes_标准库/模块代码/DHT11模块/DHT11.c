#include "DHT11.h"

/*
@brief:DHT11初始化
@param:无
@retval:无
*/
void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCCDHT11,ENABLE);	//开启GPIOA时钟
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = DHTMode;		//开漏输出
	GPIO_Initstructure.GPIO_Pin = DHT11_IO;				//GPIOPA7
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHTGPIO,&GPIO_Initstructure);					//初始化GPIOPA7
	
	GPIO_SetBits(DHTGPIO,DHT11_IO);							//默认高电平
}

/*
@brief:置PA7高低电平
@param:BitValue:1|0
@retval:无
*/
void WriteIO(uint8_t BitValue)
{
	GPIO_WriteBit(DHTGPIO,DHT11_IO,(BitAction)BitValue);
}

/*
@brief:DHT11起始信号
@param:无
@retval:无
*/
void DHT11_Start()
{
	WriteIO(0);		//主机拉低总线
	Delay_ms(20);	//至少拉低18ms
	WriteIO(1);		//释放总线
	Delay_us(30);	//释放总线20~40us
}

/*
@brief:起始信号以后检测DHT11的响应信号
@param:无
@retval:stateData,1表示DHT11响应,2表示DHT11无响应
*/
uint8_t DHT11_Check()
{
	uint8_t stateData = 0;
	if(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO) == RESET)//DHT11收到起始信号后会把总线拉低
	{
		Delay_us(80);//等待80us
		if(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO) == SET)//DHT11拉高表示响应信号有用
		{
			stateData = 1;
		}
		else
		{
			stateData = 2;
		}
		while(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO) == SET);//引脚恢复低电平则表示传感器准备发数据
	}
	return stateData;
}
/*
@brief:接收DHT11传感器的一个字节数据
@param:无
@retval:Byte:接收的数据
*/
uint8_t DHT11_ReadByte()
{
	uint8_t i,Byte = 0x00;// i是位数 byte是接受的字节
	for(i=0;i<8;i++)
	{
		while(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO) == RESET);//DHT11每发一位数据之前,都先拉低,所以等待总线拉高
		Delay_us(40);//等待40us
		//高电平持续时间26-28us为0,70us为1
		if(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO))
		{
			Byte |= (0x80>>i);//高位在前
		}
		while(GPIO_ReadInputDataBit(DHTGPIO,DHT11_IO) == SET);//等待高电平结束,为下一次接收数据做准备
	}
	return Byte;
}

/*
@brief:接收DHT11传感器的温度和湿度
@param1:
@param1:
@retval:Byte:接收的数据
*/
uint8_t DHT11_ReadData(uint8_t *Hum,uint8_t *FHum,uint8_t *Temp,uint8_t *FTemp)
{
	uint8_t Buf[5];//传来的40bit数据
	uint8_t i,Flag = 0;
	DHT11_Start();//起始信号
	if(DHT11_Check() == 1)//DHT11正确响应
	{
		for(i=0;i<5;i++) //循环读取五次 五个字节
		{
			Buf[i] = DHT11_ReadByte();//读取5个字节存在数组中
		}
		Delay_us(60);//保证完整读取
		if(Buf[0] + Buf[1] + Buf[2] + Buf[3] == Buf[4])//校验数据是否有效,如果有效
		{
			*Hum = Buf[0];		//湿度整数部分
			*FHum = Buf[1];		//湿度小数部分
			*Temp = Buf[2];		//温度整数部分
			*FTemp = Buf[3];	//温度小数部分
			Flag = 2;			//验证读取数据是否正确
			return Flag;
		}
		else//校验失败
		{
			*Hum = 0xFF;
			*FHum = 0xFF;
			*Temp = 0xFF;
			*FTemp = 0xFF;
		}
	}
	else//DHT11无响应
	{
		*Hum = 0xFF;
		*FHum = 0xFF;
		*Temp = 0xFF;
		*FTemp = 0xFF;
	}
}