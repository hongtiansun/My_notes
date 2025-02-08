#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

#define	MPU6050_SMPLRT_DIV		0x19 //采样率分频器
#define	MPU6050_CONFIG			0x1A //MPU6050配置寄存器
#define	MPU6050_GYRO_CONFIG		0x1B //陀螺仪配置寄存器
#define	MPU6050_ACCEL_CONFIG	0x1C //加速度计配置寄存器

#define	MPU6050_ACCEL_XOUT_H	0x3B //加速度计X轴输出寄存器高位
#define	MPU6050_ACCEL_XOUT_L	0x3C //加速度计X轴输出寄存器低位
#define	MPU6050_ACCEL_YOUT_H	0x3D //加速度计Y轴输出寄存器高位
#define	MPU6050_ACCEL_YOUT_L	0x3E //加速度计Y轴输出寄存器低位
#define	MPU6050_ACCEL_ZOUT_H	0x3F //加速度计Z轴输出寄存器高位
#define	MPU6050_ACCEL_ZOUT_L	0x40 //加速度计Z轴输出寄存器低位
#define	MPU6050_TEMP_OUT_H		0x41 //温度计输出寄存器高位
#define	MPU6050_TEMP_OUT_L		0x42 //温度计输出寄存器低位
#define	MPU6050_GYRO_XOUT_H		0x43 //陀螺仪X轴输出寄存器高位
#define	MPU6050_GYRO_XOUT_L		0x44 //陀螺仪X轴输出寄存器低位
#define	MPU6050_GYRO_YOUT_H		0x45 //陀螺仪Y轴输出寄存器高位
#define	MPU6050_GYRO_YOUT_L		0x46 //陀螺仪Y轴输出寄存器低位
#define	MPU6050_GYRO_ZOUT_H		0x47 //陀螺仪Z轴输出寄存器高位
#define	MPU6050_GYRO_ZOUT_L		0x48 //陀螺仪Z轴输出寄存器低位

#define	MPU6050_PWR_MGMT_1		0x6B //电源管理寄存器1
#define	MPU6050_PWR_MGMT_2		0x6C //电源管理寄存器2
#define	MPU6050_WHO_AM_I		0x75 //设备地址

#endif

/*
MPU6050_PWR_MGMT_1 (0x6B)：
用于电源管理，控制设备的唤醒和时钟源选择。
你需要将其电源管理寄存器（PWR_MGMT_1）的睡眠位（bit 6）清零
默认值：0x40（设备处于睡眠模式）。

MPU6050_PWR_MGMT_2 (0x6C)
用于进一步的电源管理，控制加速度计和陀螺仪的启用/禁用。
默认值：0x00（所有传感器均处于启用状态）。

MPU6050_SMPLRT_DIV (0x19)：
用于设置采样率分频器。
采样率 = 陀螺仪输出率 / (1 + SMPLRT_DIV)。
默认值：0x07（采样率为1kHz）

MPU6050_CONFIG (0x1A)：
用于配置数字低通滤波器（DLPF）和外部帧同步。
默认值：0x00（DLPF禁用）。

MPU6050_GYRO_CONFIG (0x1B)：
用于配置陀螺仪的量程。
默认值：0x00（±250度/秒）。

MPU6050_ACCEL_CONFIG (0x1C)：
用于配置加速度计的量程。
默认值：0x00（±2g）。

*/