#include "MyDMA.h"            // Device header

uint16_t MyDMA_Size;//数据传输大小
void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	MyDMA_Size = Size;
	//使能DMA时钟
	RCC_AHBPeriphClockCmd(RCCDMA, ENABLE);
	//DMA配置
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA; //外设地址 原地址/目标地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMAPeriphDataSize; //外设数据大小 可选参数有DMA_PeripheralDataSize_HalfWord DMA_PeripheralDataSize_Word
	DMA_InitStructure.DMA_PeripheralInc = DMAPeriphInc; //外设地址自增 可选参数有DMA_PeripheralInc_Disable
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB; //内存地址 原地址/目标地址
	DMA_InitStructure.DMA_MemoryDataSize = DMAMemoryDataSize; //内存数据大小 可选参数有DMA_MemoryDataSize_HalfWord DMA_MemoryDataSize_Word
	DMA_InitStructure.DMA_MemoryInc = DMAMemoryInc; //内存地址自增 可选参数有DMA_MemoryInc_Disable
	DMA_InitStructure.DMA_DIR = DMADIR; //数据传输方向 可选参数有DMA_DIR_PeripheralDST
	
	DMA_InitStructure.DMA_BufferSize = Size; //数据传输次数 对应于开辟的缓冲区大小
	DMA_InitStructure.DMA_Mode = DMAMode; //DMA模式 可选参数有DMA_Mode_Circular  停止还是重新开始
	DMA_InitStructure.DMA_M2M = DMAM2M; //内存到内存传输 可选参数有DMA_M2M_Disable 内存到内存支持与否
	DMA_InitStructure.DMA_Priority = DMAPriority; //DMA优先级 可选参数有DMA_Priority_Low DMA_Priority_Medium DMA_Priority_High DMA_Priority_VeryHigh
	DMA_Init(DMAChannel, &DMA_InitStructure);
	
	DMA_Cmd(DMAChannel, DISABLE);
}

void MyDMA_Transfer(void) //启动DMA传输
{
	DMA_Cmd(DMAChannel, DISABLE); //通道关闭
	DMA_SetCurrDataCounter(DMAChannel, MyDMA_Size);//设置数据传输次数
	DMA_Cmd(DMAChannel, ENABLE); //通道打开
	
	while (DMA_GetFlagStatus(DMATC) == RESET);
	DMA_ClearFlag(DMATC);
}
