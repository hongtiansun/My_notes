#include "AD.h"              // Device header

uint16_t AD_Value[4];//AD采集值定义 分配内存

void AD_Init(void)
{
	//时钟配置
	RCC_APB2PeriphClockCmd(RCCADC, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCCDMA, ENABLE);
	//ADC分频
	RCC_ADCCLKConfig(ADCCLKDiv);
	//GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIOPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	//ADC通道配置
	ADC_RegularChannelConfig(ADC, ADCCh0, ADCRank0, ADCSampleTime0);
	ADC_RegularChannelConfig(ADC, ADCCh1, ADCRank1, ADCSampleTime1);
	ADC_RegularChannelConfig(ADC, ADCCh2, ADCRank2, ADCSampleTime2);
	ADC_RegularChannelConfig(ADC, ADCCh3, ADCRank3, ADCSampleTime3);
	//ADC配置
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADCMode;
	ADC_InitStructure.ADC_DataAlign = ADCDataAlign;
	ADC_InitStructure.ADC_ExternalTrigConv = ADCExternalTrigConv;
	ADC_InitStructure.ADC_ContinuousConvMode = ADCContinuMode;
	ADC_InitStructure.ADC_ScanConvMode = ADCScanMode;
	ADC_InitStructure.ADC_NbrOfChannel = ADCNbrOfChannel;
	ADC_Init(ADC, &ADC_InitStructure);
	//DMA配置
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = DMAPeriphBaseAddr;
	DMA_InitStructure.DMA_PeripheralDataSize = DMAPeriphDataSize;
	DMA_InitStructure.DMA_PeripheralInc = DMAPeriphInc;
	DMA_InitStructure.DMA_MemoryBaseAddr = DMAMemoryBaseAddr;
	DMA_InitStructure.DMA_MemoryDataSize = DMAMemoryDataSize;
	DMA_InitStructure.DMA_MemoryInc = DMAMemoryInc;
	DMA_InitStructure.DMA_DIR = DMADIR;
	DMA_InitStructure.DMA_BufferSize = DMABufferSize;
	DMA_InitStructure.DMA_Mode = DMAMode;
	DMA_InitStructure.DMA_M2M = DMAM2M;
	DMA_InitStructure.DMA_Priority = DMAPriority;
	DMA_Init(DMAChannel, &DMA_InitStructure);
	
	DMA_Cmd(DMAChannel, ENABLE); //使能DMA
	ADC_DMACmd(ADC, ENABLE); //使能ADC的DMA
	ADC_Cmd(ADC, ENABLE); //使能ADC
	
	//ADC校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC) == SET);
	ADC_StartCalibration(ADC);
	while (ADC_GetCalibrationStatus(ADC) == SET);
	//ADC启动
	ADC_SoftwareStartConvCmd(ADC, ENABLE);
}
//DMA触发三个事件
//1.DMA使能
//2.有触发源
//3.计数器大于0 
