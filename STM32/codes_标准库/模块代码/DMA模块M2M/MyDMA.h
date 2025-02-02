#ifndef __MYDMA_H
#define __MYDMA_H

#include "stm32f10x.h"      
//时钟接口
#define RCCDMA RCC_AHBPeriph_DMA1
//外设接口
#define DMAPeriphDataSize DMA_PeripheralDataSize_Byte
#define DMAPeriphInc DMA_PeripheralInc_Enable
//内存接口
#define DMAMemoryDataSize DMA_MemoryDataSize_Byte
#define DMAMemoryInc DMA_MemoryInc_Enable
//其他配置
#define DMADIR DMA_DIR_PeripheralSRC
#define DMAMode DMA_Mode_Normal
#define DMAM2M DMA_M2M_Enable
#define DMAPriority DMA_Priority_Medium
//通道配置
#define DMAChannel DMA1_Channel1
//标志位
#define DMATC DMA1_FLAG_TC1

void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size);
void MyDMA_Transfer(void);

#endif
