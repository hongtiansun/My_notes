#include "MPU6050.h"

#define MPU6050_ADDRESS		0xD0 //MPU6050地址
//TIM接口
#define TIMRCC RCC_APB1Periph_TIM2
#define TIM TIM2
#define TIMClockDivision TIM_CKD_DIV1 //可选参数 TIM_CKD_DIV2 TIM_CKD_DIV4
#define TIMCounterMode TIM_CounterMode_Up //可选参数 TIM_CounterMode_Down
#define TIMARR 10000-1
#define TIMPSC 72-1 // 频率72000 000 / 72 = 1000 000Hz 1us

uint32_t lastTime,currentTime;
float deltaTime;
/*
@brief: 指定地址写入数据
@param: RegAddress: 寄存器地址
		Data: 要写入的数据
@retval: void
*/

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start(); //开始信号
	MyI2C_SendByte(MPU6050_ADDRESS); //发送地址
	MyI2C_ReceiveAck(); //接收应答
	MyI2C_SendByte(RegAddress); //发送寄存器地址
	MyI2C_ReceiveAck(); //接收应答
	MyI2C_SendByte(Data); //发送数据
	MyI2C_ReceiveAck(); //接收应答
	MyI2C_Stop(); //结束信号
}
/*
@brief：读取指定地址的数据
@param: RegAddress: 寄存器地址
@retval: uint8_t: 读取到的数据
*/

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data; //数据
	
	MyI2C_Start(); //开始信号
	MyI2C_SendByte(MPU6050_ADDRESS); //发送地址
	MyI2C_ReceiveAck(); //接收应答
	MyI2C_SendByte(RegAddress); //发送寄存器地址
	MyI2C_ReceiveAck(); //接收应答
	
	MyI2C_Start(); //不停止直接重开
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01); //发送地址 配上读写位 1为读 0为写
	MyI2C_ReceiveAck(); //接收应答
	Data = MyI2C_ReceiveByte(); //接收数据
	MyI2C_SendAck(1); //发送非应答
	MyI2C_Stop(); //结束信号
	
	return Data;
}

/*
@brief: 初始化MPU6050
@param: void
@retval: void
*/

void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01); //唤醒设备
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00); //启用所有传感器
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09); //设置采样率为100Hz
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);  //配置数字低通滤波器
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); //设置陀螺仪量程为2000度/秒 1000度/秒为0x10 500度/秒为0x08 250度/秒为0x00
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18); //设置加速度计量程为16g 8g为0x10 4g为0x08 2g为0x00
}
/*
@brief: 获取MPU6050的ID
@param: void
@retval: uint8_t: ID
*/
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
/*
@brief: 获取MPU6050的数据
@param: AccX: 加速度计X轴数据
		AccY: 加速度计Y轴数据
		AccZ: 加速度计Z轴数据
		GyroX: 陀螺仪X轴数据
		GyroY: 陀螺仪Y轴数据
		GyroZ: 陀螺仪Z轴数据
@retval: void
*/
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL; //16位数据
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL; //16位数据
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL; //16位数据
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL; //16位数据
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL; //16位数据
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL; //16位数据
}
/*
@brief：获取MPU6050的温度
@param: void
@retval: uint16_t: 温度
*/
uint16_t MPU6050_GetTemp(void)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_TEMP_OUT_H);
	DataL = MPU6050_ReadReg(MPU6050_TEMP_OUT_L);
	
	return (DataH << 8) | DataL;
}

void MPU6050_Euler(float* Roll,float* Pitch,float* Yaw)
{
	int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
	float AccXf, AccYf, AccZf, GyroXf, GyroYf, GyroZf;
	float Rollf, Pitchf, Yawf;
	currentTime = TIM_GetCounter(TIM); //获取当前时间	
	deltaTime = (currentTime - lastTime) ;
	if(deltaTime < 0) deltaTime += 10000.0; //借位处理 自动重装寄存器为1000 // 0 1 2 3  3-0=3  /  8 9 0 1 1-8=-7+10=3
	deltaTime = deltaTime / 1000000.0; //转换为秒 1us到秒

	lastTime = currentTime;
	MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
	
	AccXf = AccX / 16384.0;
	AccYf = AccY / 16384.0;
	AccZf = AccZ / 16384.0;
	
	GyroXf = GyroX / 131.0;
	GyroYf = GyroY / 131.0;
	GyroZf = GyroZ / 131.0;
	
	Rollf = atan2(AccYf, AccZf) * 180 / 3.1415926; //弧度转角度
	Pitchf = atan2(-AccXf, sqrt(AccYf * AccYf + AccZf * AccZf)) * 180 / 3.1415926; //弧度转角度
	Yawf = *Yaw + GyroZf * deltaTime;
	
	*Roll = Rollf;
	*Pitch = Pitchf;
	*Yaw = Yawf; //计算不准 后续优化
}

void Time_Init(void)
{
	//开启时钟 TIM2 挂载在APB1总线上
	RCC_APB1PeriphClockCmd(TIMRCC,ENABLE);

	//选择时基时钟 内部时钟
	TIM_InternalClockConfig(TIM);

	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIMClockDivision;//采样频率分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIMCounterMode;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=TIMARR;//ARR自动重装寄存器
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIMPSC;//PSC预分频器值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器的值 高级定时器才有
	TIM_TimeBaseInit(TIM,&TIM_TimeBaseInitStructure);

	TIM_Cmd(TIM,ENABLE);
}

