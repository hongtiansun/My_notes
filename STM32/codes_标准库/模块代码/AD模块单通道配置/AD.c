#include "AD.h"
//AD初始化
void AD_Init(void)
{
	//使能ADC1和GPIOA时钟
	RCC_APB2PeriphClockCmd(RCCADC, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	//ADC时钟设置为PCLK2的6分频，即12MHz
	RCC_ADCCLKConfig(ADCCLKDiv);
	
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIOPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);
	
	//ADC配置
	ADC_RegularChannelConfig(ADC, ADCChannel, ADCRank, ADCSampleTime);//可配置多个通道 实现连续装换
	/*ADCx：指定要配置的 ADC 外设。这个参数可以是以下值之一：
	ADC1：表示 ADC1 外设
	ADC2：表示 ADC2 外设（如果存在）
	ADC3：表示 ADC3 外设（如果存在）

	ADC_Channel：指定要配置的 ADC 通道。这个参数可以是以下值之一：
	ADC_Channel_0 到 ADC_Channel_18：表示 ADC 的通道 0 到通道 18（具体通道数量取决于具体的 STM32 型号）

	Rank：指定在常规组序列中的排名。指定了转换优先级
	这个参数的取值范围是 1 到 16，表示该通道在常规组中的顺序。
	
	ADC_SampleTime：指定 ADC 通道的采样时间。这个参数可以是以下值之一：
	ADC_SampleTime_1Cycles5：1.5 个 ADC 时钟周期
	ADC_SampleTime_7Cycles5：7.5 个 ADC 时钟周期
	ADC_SampleTime_13Cycles5：13.5 个 ADC 时钟周期
	ADC_SampleTime_28Cycles5：28.5 个 ADC 时钟周期
	ADC_SampleTime_41Cycles5：41.5 个 ADC 时钟周期
	ADC_SampleTime_55Cycles5：55.5 个 ADC 时钟周期
	ADC_SampleTime_71Cycles5：71.5 个 ADC 时钟周期
	ADC_SampleTime_239Cycles5：239.5 个 ADC 时钟周期
	*/
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADCMode;
	ADC_InitStructure.ADC_DataAlign = ADCDataAlign;
	ADC_InitStructure.ADC_ExternalTrigConv = ADCExternalTrigConv;
	/*ADC_ExternalTrigConv_T1_CC1

	由定时器 1 的捕获比较 1 事件触发。
	ADC_ExternalTrigConv_T1_CC2

	由定时器 1 的捕获比较 2 事件触发。
	ADC_ExternalTrigConv_T1_CC3

	由定时器 1 的捕获比较 3 事件触发。
	ADC_ExternalTrigConv_T2_CC2

	由定时器 2 的捕获比较 2 事件触发。
	ADC_ExternalTrigConv_T3_TRGO

	由定时器 3 的触发输出事件触发。
	ADC_ExternalTrigConv_T4_CC4

	由定时器 4 的捕获比较 4 事件触发。
	ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO

	由外部中断线 11 或定时器 8 的触发输出事件触发。
	ADC_ExternalTrigConv_None

	不使用外部触发，转换由软件触发。*/
	ADC_InitStructure.ADC_ContinuousConvMode = ADCContinuMode;
	ADC_InitStructure.ADC_ScanConvMode = ADCScanMode;
	ADC_InitStructure.ADC_NbrOfChannel = ADCNbrOfChannel;
	ADC_Init(ADC, &ADC_InitStructure);
	ADC_Cmd(ADC, ENABLE);//使能ADC1
	
	//校准
	ADC_ResetCalibration(ADC);//复位校准
	while (ADC_GetResetCalibrationStatus(ADC) == SET);//等待校准复位完成
	ADC_StartCalibration(ADC);//开始校准
	while (ADC_GetCalibrationStatus(ADC) == SET);//等待校准完成
}

uint16_t AD_GetValue(void)
{
	ADC_SoftwareStartConvCmd(ADC, ENABLE);//软件触发 开始转换
	while (ADC_GetFlagStatus(ADC, ADC_FLAG_EOC) == RESET);//等待转换完成 EOC自动清零
	return ADC_GetConversionValue(ADC);//返回转换结果
}
