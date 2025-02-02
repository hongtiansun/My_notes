#include "Encoder.h"

void Encoder_Init(void)
{
	//使能时钟
	RCC_APB1PeriphClockCmd(RCCTIM, ENABLE);
	RCC_APB2PeriphClockCmd(RCCGPIO, ENABLE);
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = EnGPIOA | EnGPIOB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EnGPIO, &GPIO_InitStructure);
	//TIM初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不起作用
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//不起作用
	TIM_TimeBaseInitStructure.TIM_Period = TIMARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = TIMPSC;		//不起作用Encode接管
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM, &TIM_TimeBaseInitStructure);
	//IC初始化
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = ICChannelA;
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;
	TIM_ICInit(TIM, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = ICChannelB;
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;
	TIM_ICInit(TIM, &TIM_ICInitStructure);
	//编码器初始化
	TIM_EncoderInterfaceConfig(TIM, EncoderMode, EncoderPolarityA, EncoderPolarityB);
	/**
	 * @brief  Configures the TIMx Encoder Interface.
	 * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
	 * @param  TIM_EncoderMode: specifies the TIMx Encoder Mode.
	 *   This parameter can be one of the following values:
	 *     @arg TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
	 *     @arg TIM_EncoderMode_TI2: Counter counts on TI2FP2 edge depending on TI1FP1 level.
	 *     @arg TIM_EncoderMode_TI12: Counter counts on both TI1FP1 and TI2FP2 edges depending
	 *                                on the level of the other input.
	 * @param  TIM_IC1Polarity: specifies the IC1 Polarity
	 *   This parameter can be one of the following values:
	 *     @arg TIM_ICPolarity_Falling: IC Falling edge.
	 *     @arg TIM_ICPolarity_Rising: IC Rising edge.
	 * @param  TIM_IC2Polarity: specifies the IC2 Polarity
	 *   This parameter can be one of the following values:
	 *     @arg TIM_ICPolarity_Falling: IC Falling edge.
	 *     @arg TIM_ICPolarity_Rising: IC Rising edge.
	 * @retval None
	 */
	TIM_Cmd(TIM, ENABLE);
}

int16_t Encoder_Get(void)
{
	/*
	int16_t Temp;
	Temp = TIM_GetCounter(TIM);//读取计数器CNT
	TIM_SetCounter(TIM, 0); //清零计数器CNT
	return Temp;*/
	return TIM_GetCounter(TIM)/4;
}
