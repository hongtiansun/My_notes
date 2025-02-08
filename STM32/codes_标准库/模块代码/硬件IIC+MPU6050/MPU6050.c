#include "MPU6050.h"

/*
@brief: 等待IIC事件 在事件到达前，持续进入循环等待，直到事件到达或超时
@param: I2Cx: I2C外设
@param: I2C_EVENT: I2C事件
@retval: None
*/
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000; // 超时时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
}
/*
@brief: Write data to MPU6050 register
@param: RegAddress: Register address
@param: Data: Data to write
@retval: None
*/
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	I2C_GenerateSTART(IIC, ENABLE); // 发送开始信号
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_MODE_SELECT); //等待Ev5事件 表示发送开始信号成功
	
	I2C_Send7bitAddress(IIC, MPU6050_ADDRESS, I2C_Direction_Transmitter);//发送器件地址 硬件自动发送接收应答
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED); //等待Ev6事件 表示发送器件地址成功
	
	I2C_SendData(IIC, RegAddress); //发送寄存器地址
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_TRANSMITTING);//等待Ev8事件 表示发送寄存器地址成功
	
	I2C_SendData(IIC, Data);//发送数据(存放在DR中)
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_TRANSMITTED);//等待Ev8_2事件 表示开始产生停止信号
	
	I2C_GenerateSTOP(IIC, ENABLE); //发送停止信号
}
/*
@brief: Read data from MPU6050 register
@param: RegAddress: Register address
@retval: Data read
*/
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;//读取的数据
	
	I2C_GenerateSTART(IIC, ENABLE);//发送开始信号
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_MODE_SELECT);//等待Ev5事件 表示发送开始信号成功
	
	I2C_Send7bitAddress(IIC, MPU6050_ADDRESS, I2C_Direction_Transmitter);//发送器件地址
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//等待Ev6事件 表示发送器件地址成功 发送Ev6
	
	I2C_SendData(IIC, RegAddress);//发送寄存器地址
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_TRANSMITTED);//等待Ev8_2事件 表示此次发送完成 可以开始停止
	
	I2C_GenerateSTART(IIC, ENABLE); //再次发送开始信号
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_MODE_SELECT);//等待Ev5事件 表示发送开始信号成功
	
	I2C_Send7bitAddress(IIC, MPU6050_ADDRESS, I2C_Direction_Receiver);//发送器件地址 接收模式
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//等到Ev6事件 表示发送器件地址成功 接收Ev6
	
	I2C_AcknowledgeConfig(IIC, DISABLE);//关闭应答 不应答表示接收到此数据后不再接收下一个数据 提前将ACK使能否则在接受完数据后自动又一次应答了
	I2C_GenerateSTOP(IIC, ENABLE); //发送停止信号 会自动延迟到当前数据接收完发送停止信号
	
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_RECEIVED);//等待Ev7事件 表示接收到数据 
	Data = I2C_ReceiveData(IIC);//读取数据清楚EV7事件
	
	I2C_AcknowledgeConfig(IIC, ENABLE);//恢复使能，默认状态就是使能，此举只是恢复默认
	
	return Data; //返回读取的数据
	/*
	要接受n个字节
	前面字节n-1个字节只需要执行
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_RECEIVED);//等待Ev7事件 表示接收到数据 
	Data = I2C_ReceiveData(IIC);//读取数据清楚EV7事件
	到最后一个字节
	I2C_AcknowledgeConfig(IIC, DISABLE);//关闭应答 不应答表示接收到此数据后不再接收下一个数据 提前将ACK使能否则在接受完数据后自动又一次应答了
	I2C_GenerateSTOP(IIC, ENABLE); //发送停止信号 会自动延迟到当前数据接收完发送停止信号
	MPU6050_WaitEvent(IIC, I2C_EVENT_MASTER_BYTE_RECEIVED);//等待Ev7事件 表示接收到数据 
	Data = I2C_ReceiveData(IIC);//读取数据清楚EV7事件
	*/
}

void MPU6050_Init(void)
{
	//开启时钟
	RCC_APB1PeriphClockCmd(RCCIIC, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //复用开漏输出
	GPIO_InitStructure.GPIO_Pin = SDA | SCL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //I2C模式 其余SMBus模式
	I2C_InitStructure.I2C_ClockSpeed = IICSpeed; //时钟速度 SCL的时钟频率 0 - 400KHZ
	I2C_InitStructure.I2C_DutyCycle = IICDuty; //占空比 0-100 保持1:1 100KHZ以上起作用 有低比高 2:1 16:9选项
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; //默认应答
	I2C_InitStructure.I2C_AcknowledgedAddress = IICAddress; //7位地址
	I2C_InitStructure.I2C_OwnAddress1 = 0x00; //自身地址 从模式使用 本出不适用
	I2C_Init(IIC, &I2C_InitStructure);
	//开启IIC
	I2C_Cmd(IIC, ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

/*
@brief：获取MPU6050的温度
@param: void
@retval: uint16_t: 温度
*/
float MPU6050_GetTemp(void)
{
	uint8_t DataH, DataL;
	float temp;
	DataH = MPU6050_ReadReg(MPU6050_TEMP_OUT_H);
	DataL = MPU6050_ReadReg(MPU6050_TEMP_OUT_L);
	temp= (DataH << 8) | DataL;
	temp = temp / 340.0 +36.53;
	return temp;
}

void MPU6050_GetEula(float *pitch,float *roll){
	int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
	float AccXf, AccYf, AccZf, GyroXf, GyroYf, GyroZf;
	float Rollf, Pitchf;

	MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
	
	AccXf = AccX / 16384.0;
	AccYf = AccY / 16384.0;
	AccZf = AccZ / 16384.0;
	
	GyroXf = GyroX / 131.0;
	GyroYf = GyroY / 131.0;
	GyroZf = GyroZ / 131.0;
	
	Rollf = atan2(AccYf, AccZf) * 180 / 3.1415926; //弧度转角度
	Pitchf = atan2(-AccXf, sqrt(AccYf * AccYf + AccZf * AccZf)) * 180 / 3.1415926; //弧度转角度
	
	*roll = Rollf;
	*pitch = Pitchf;
}
