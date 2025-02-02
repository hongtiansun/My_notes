#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "OneWire.h"
#include "stm32f10x.h"

//DS18B20指令
#define DS18B20_SKIP_ROM			0xCC //跳过ROM
#define DS18B20_CONVERT_T			0x44 //温度转换
#define DS18B20_READ_SCRATCHPAD 	0xBE //读取暂存器

void DS18B20_ConvertT(void);
float DS18B20_ReadT(void);

#endif