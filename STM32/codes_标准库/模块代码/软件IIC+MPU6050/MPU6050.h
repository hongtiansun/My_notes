#ifndef __MPU6050_H
#define __MPU6050_H
#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#include "math.h"

extern uint32_t lastTime;
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);

void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
uint16_t MPU6050_GetTemp(void);
void MPU6050_Euler(float* Roll,float* Pitch,float* Yaw);
void Time_Init(void);
#endif
