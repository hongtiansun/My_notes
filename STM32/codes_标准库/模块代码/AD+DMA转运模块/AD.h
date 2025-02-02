#ifndef __AD_H
#define __AD_H
#include "stm32f10x.h"                  // Device header
extern uint16_t AD_Value[4];//AD采集值声明

//AD接口
//RCC接口
#define RCCADC RCC_APB2Periph_ADC1
#define RCCGPIO RCC_APB2Periph_GPIOA
#define ADCCLKDiv RCC_PCLK2_Div6
//GPIO接口
#define GPIO GPIOA
#define GPIOPin GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
//ADC通道接口
#define ADC ADC1
#define ADCCh0 ADC_Channel_0
#define ADCCh1 ADC_Channel_1
#define ADCCh2 ADC_Channel_2
#define ADCCh3 ADC_Channel_3
#define ADCRank0 1
#define ADCRank1 2
#define ADCRank2 3
#define ADCRank3 4
#define ADCSampleTime0 ADC_SampleTime_55Cycles5
#define ADCSampleTime1 ADC_SampleTime_55Cycles5
#define ADCSampleTime2 ADC_SampleTime_55Cycles5
#define ADCSampleTime3 ADC_SampleTime_55Cycles5
//ADC配置接口
#define ADCMode ADC_Mode_Independent //独立模式 ADC独立工作 可选参数ADC_Mode_RegInjecSimult
#define ADCDataAlign ADC_DataAlign_Right //右对齐
#define ADCExternalTrigConv ADC_ExternalTrigConv_None //不使用外部触发 软件控制

#define ADCContinuMode ENABLE //连续转换
#define ADCScanMode ENABLE //多通道转换
#define ADCNbrOfChannel 4 //转换通道数目 只转换一个通道

//DMA接口
//时钟接口
#define RCCDMA RCC_AHBPeriph_DMA1
//外设接口
#define DMAPeriphBaseAddr (uint32_t)&ADC1->DR
#define DMAPeriphDataSize DMA_PeripheralDataSize_HalfWord
#define DMAPeriphInc DMA_PeripheralInc_Disable
//内存接口
#define DMAMemoryBaseAddr (uint32_t)AD_Value
#define DMAMemoryDataSize DMA_MemoryDataSize_HalfWord
#define DMAMemoryInc DMA_MemoryInc_Enable
//其他配置
#define DMADIR DMA_DIR_PeripheralSRC
#define DMABufferSize 4 //隐含了传输次数 计数器值
#define DMAMode DMA_Mode_Circular //循环模式 
#define DMAM2M DMA_M2M_Disable
#define DMAPriority DMA_Priority_Medium
//通道配置
#define DMAChannel DMA1_Channel1 //ADC1使用DMA1通道1
//标志位
#define DMATC DMA1_FLAG_TC1

void AD_Init(void);

#endif
