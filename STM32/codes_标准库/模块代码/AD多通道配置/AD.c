#include "AD.h"

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCCADC, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	
	RCC_ADCCLKConfig(ADCDiv);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIOPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADCMode;
	ADC_InitStructure.ADC_DataAlign = ADCDataAlign;
	ADC_InitStructure.ADC_ExternalTrigConv = ADCExternalTrigConv;
	ADC_InitStructure.ADC_ContinuousConvMode = ADCContinuMode;
	ADC_InitStructure.ADC_ScanConvMode = ADCScanMode;
	ADC_InitStructure.ADC_NbrOfChannel = ADCNbChannel; //转化通道数目
	ADC_Init(ADC, &ADC_InitStructure);
	
	ADC_Cmd(ADC, ENABLE); //使能ADC
	
	ADC_ResetCalibration(ADC);
	while (ADC_GetResetCalibrationStatus(ADC) == SET);
	ADC_StartCalibration(ADC);
	while (ADC_GetCalibrationStatus(ADC) == SET);
}

uint16_t AD_GetValue(uint8_t ADC_Channel) //参数为通道号 可选值为0~15 ADC_Channel_0~ADC_Channel_15
{
	ADC_RegularChannelConfig(ADC, ADC_Channel, 1, ADCSampTime);
	ADC_SoftwareStartConvCmd(ADC, ENABLE);//软件触发转换
	while (ADC_GetFlagStatus(ADC, ADC_FLAG_EOC) == RESET);//等待转换结束
	return ADC_GetConversionValue(ADC);//返回转换结果
}
