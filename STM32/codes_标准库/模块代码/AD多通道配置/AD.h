#ifndef __AD_H
#define __AD_H
#include "stm32f10x.h"                  // Device header

//RCC接口
#define RCCADC RCC_APB2Periph_ADC1
#define RCCGPIO RCC_APB2Periph_GPIOA
#define ADCDiv RCC_PCLK2_Div6 //ADC时钟分频
//GPIO接口
#define GPIO GPIOA
#define GPIOPin GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
//ADC接口
#define ADCMode ADC_Mode_Independent
#define ADCDataAlign ADC_DataAlign_Right
#define ADCExternalTrigConv ADC_ExternalTrigConv_None
#define ADCContinuMode DISABLE
#define ADCScanMode DISABLE
#define ADCNbChannel 1
#define ADC ADC1
#define ADCSampTime ADC_SampleTime_55Cycles5

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);

#endif
