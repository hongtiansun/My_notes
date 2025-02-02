#ifndef __AD_H
#define __AD_H
#include "stm32f10x.h"                  // Device header

//RCC接口
#define RCCADC RCC_APB2Periph_ADC1
#define RCCGPIO RCC_APB2Periph_GPIOA
#define ADCCLKDiv RCC_PCLK2_Div6
//GPIO接口
#define GPIO GPIOA
#define GPIOPin GPIO_Pin_0
//ADC通道接口
#define ADC ADC1
#define ADCChannel ADC_Channel_0
#define ADCRank 1
#define ADCSampleTime ADC_SampleTime_55Cycles5
//ADC配置接口
#define ADCMode ADC_Mode_Independent //独立模式 ADC独立工作 可选参数ADC_Mode_RegInjecSimult
#define ADCDataAlign ADC_DataAlign_Right //右对齐
#define ADCExternalTrigConv ADC_ExternalTrigConv_None //不使用外部触发 软件控制

#define ADCContinuMode DISABLE //单次转换
#define ADCScanMode DISABLE //单通道转换  --对应于单次不扫描
#define ADCNbrOfChannel 1 //转换通道数目 只转换一个通道
void AD_Init(void);
uint16_t AD_GetValue(void);

#endif
